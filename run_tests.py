#!/usr/bin/env python3
# Copyright (C) 2023-2026, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only

"""
Global test runner for VerifMSI++.

Builds the library, compiles and runs all unit test suites under tests/,
tests the Python bindings, and optionally runs the hardware/software benchmarks
against their reference results.
"""

import argparse
import concurrent.futures
import os
import re
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Tuple


ROOT_DIR = Path(__file__).resolve().parent

UNIT_TEST_SUITES = [
    "simplify",
    "extended_simplify",
    "large_int",
    "arith",
    "concrev",
    "ni",
    "tps/OK",
    "tps/KO",
    "mask_occ",
]

# Regular expressions to match standalone OK / KO tokens in test output
OK_REGEX = re.compile(r"(?:^|\s|\[)OK(?:\]|\s|:|$)", re.MULTILINE)
KO_REGEX = re.compile(r"(?:^|\s|\[)KO(?:\]|\s|:|$)", re.MULTILINE)
LEAK_ZERO_REGEX = re.compile(r"Total Nb\. of potential leakages found:\s*0\b")
LEAK_NONZERO_REGEX = re.compile(r"Total Nb\. of potential leakages found:\s*[1-9]\d*\b")


@dataclass
class TestResult:
    suite: str
    name: str
    status: str  # "PASS", "FAIL", "NO_ASSERT", "TIMEOUT"
    duration_s: float
    returncode: int
    stdout: str
    stderr: str
    reason: str = ""


def run_cmd(
    cmd: List[str],
    cwd: Path,
    timeout: Optional[float] = None,
    env: Optional[Dict[str, str]] = None,
) -> subprocess.CompletedProcess:
    return subprocess.run(
        cmd,
        cwd=str(cwd),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        timeout=timeout,
        env=env,
    )


def build_library(jobs: int, verbose: bool) -> bool:
    print("==> Building VerifMSI++ library...")
    res = run_cmd(["make", f"-j{jobs}"], cwd=ROOT_DIR)
    if res.returncode != 0:
        print(res.stdout)
        print(res.stderr, file=sys.stderr)
        print("[FAIL] Library build failed.")
        return False
    if verbose and res.stdout.strip():
        print(res.stdout.strip())
    return True


def build_test_suite(suite: str, jobs: int, verbose: bool) -> Tuple[bool, str]:
    suite_dir = ROOT_DIR / "tests" / suite
    env = os.environ.copy()
    env["VMSI_HOME"] = str(ROOT_DIR)
    res = run_cmd(["make", f"-j{jobs}"], cwd=suite_dir, env=env)
    if res.returncode != 0:
        return False, res.stdout + "\n" + res.stderr
    if verbose and res.stdout.strip():
        print(res.stdout.strip())
    return True, ""


def execute_single_binary(
    suite: str, binary_path: Path, timeout: float
) -> TestResult:
    t0 = time.perf_counter()
    try:
        res = run_cmd([str(binary_path)], cwd=binary_path.parent, timeout=timeout)
    except subprocess.TimeoutExpired as exc:
        dt = time.perf_counter() - t0
        return TestResult(
            suite=suite,
            name=binary_path.name,
            status="TIMEOUT",
            duration_s=dt,
            returncode=-1,
            stdout=exc.stdout or "" if isinstance(exc.stdout, str) else "",
            stderr=exc.stderr or "" if isinstance(exc.stderr, str) else "",
            reason=f"Timed out after {timeout}s",
        )

    dt = time.perf_counter() - t0
    out = res.stdout
    err = res.stderr

    if res.returncode != 0:
        return TestResult(
            suite=suite,
            name=binary_path.name,
            status="FAIL",
            duration_s=dt,
            returncode=res.returncode,
            stdout=out,
            stderr=err,
            reason=f"Non-zero exit code ({res.returncode})",
        )

    has_ko = bool(KO_REGEX.search(out)) or bool(LEAK_NONZERO_REGEX.search(out))
    has_ok = bool(OK_REGEX.search(out)) or bool(LEAK_ZERO_REGEX.search(out))

    if has_ko:
        return TestResult(
            suite=suite,
            name=binary_path.name,
            status="FAIL",
            duration_s=dt,
            returncode=res.returncode,
            stdout=out,
            stderr=err,
            reason="Output contains 'KO' or unexpected leakage",
        )

    if not has_ok:
        return TestResult(
            suite=suite,
            name=binary_path.name,
            status="NO_ASSERT",
            duration_s=dt,
            returncode=res.returncode,
            stdout=out,
            stderr=err,
            reason="Exited with 0 but printed no 'OK' check result",
        )

    return TestResult(
        suite=suite,
        name=binary_path.name,
        status="PASS",
        duration_s=dt,
        returncode=res.returncode,
        stdout=out,
        stderr=err,
    )


def run_python_bindings_test(timeout: float) -> TestResult:
    """Runs a unit test suite exercising the Python pybind11 wrapper."""
    code = """
import sys
sys.path.insert(0, ".")
import verif_msi_pp as vmsi

k = vmsi.symbol("k", "S", 8)
m = vmsi.symbol("m", "M", 8)
p = vmsi.symbol("p", "P", 8)
c = vmsi.constant(0x5A, 8)

# 1. Simplify & equivalence
e1 = (k ^ m) ^ m
assert vmsi.equivalence(vmsi.simplify(e1), k), "simplify((k ^ m) ^ m) != k"

# 2. TPS & NI
assert vmsi.checkTpsVal(k ^ p)[0] == False, "k ^ p should leak in TPS"
assert vmsi.checkTpsVal(k ^ m)[0] == True, "k ^ m should be TPS secure"
u = vmsi.getRealShares(k, 2)
assert vmsi.checkNIVal(u[0] ^ p, 1)[0] == True, "u0 ^ p should be 1-NI"
assert vmsi.checkNIVal(u[0] ^ u[1], 1)[0] == False, "u0 ^ u1 should not be 1-NI"

# 3. Concrete evaluation
res, v0, v1 = vmsi.compareExpsWithExev(u[0] ^ u[1], k)
assert res is None, f"compareExpsWithExev(u0 ^ u1, k) failed: {res}"

# 4. Hardware gates & checkSecurity
g_u0 = vmsi.inputGate(u[0])
g_u1 = vmsi.inputGate(u[1])
g_m = vmsi.inputGate(m)
g_masked = vmsi.xorGate(g_u0, g_m)
assert vmsi.checkSecurity(1, False, "ni", g_masked, g_u1)[0] == 0
assert vmsi.checkSecurity(1, False, "ni", vmsi.xorGate(g_u0, g_u1), g_u1)[0] > 0

print("OK")
"""
    t0 = time.perf_counter()
    env = os.environ.copy()
    env["PYTHONPATH"] = str(ROOT_DIR.parent) + os.pathsep + env.get("PYTHONPATH", "")
    res = run_cmd([sys.executable, "-c", code], cwd=ROOT_DIR, timeout=timeout, env=env)
    dt = time.perf_counter() - t0
    if res.returncode == 0 and "OK" in res.stdout:
        return TestResult(
            suite="python",
            name="pybind11_smoke",
            status="PASS",
            duration_s=dt,
            returncode=0,
            stdout=res.stdout,
            stderr=res.stderr,
        )
    return TestResult(
        suite="python",
        name="pybind11_smoke",
        status="FAIL",
        duration_s=dt,
        returncode=res.returncode,
        stdout=res.stdout,
        stderr=res.stderr,
        reason=(res.stderr.strip() or res.stdout.strip() or "Python test failed"),
    )


def run_hw_benchmarks(
    jobs: int, max_order: int, timeout: float, verbose: bool
) -> List[TestResult]:
    """Compiles and runs hardware benchmarks against benchmarks/hw/test_env/dico_ref.py."""
    print(f"\n==> Running Hardware Benchmarks (benchmarks/hw, max_order={max_order})...")
    test_env_dir = ROOT_DIR / "benchmarks" / "hw" / "test_env"
    sys.path.insert(0, str(test_env_dir))
    from dico_ref import refResult  # type: ignore

    sec_props = ["tps", "ni", "sni", "rni", "pini", "opini"]
    results: List[TestResult] = []
    hw_dir = ROOT_DIR / "benchmarks" / "hw"
    env = os.environ.copy()
    env["VMSI_HOME"] = str(ROOT_DIR)

    for bench, info in refResult.items():
        bench_dir = hw_dir / info["dir"]
        for order in range(1, max_order + 1):
            if not (info["gen"] or order == info["verif_order"]):
                continue

            if info["gen"]:
                exec_name = f"{bench}_gen_{order + 1}_shares"
                gen_res = run_cmd(
                    [sys.executable, info["gen_file"], "-n", str(order + 1)],
                    cwd=bench_dir,
                    env=env,
                )
                if gen_res.returncode != 0:
                    results.append(
                        TestResult(
                            suite="benchmarks/hw",
                            name=f"{bench} (gen order {order})",
                            status="FAIL",
                            duration_s=0.0,
                            returncode=gen_res.returncode,
                            stdout=gen_res.stdout,
                            stderr=gen_res.stderr,
                            reason="Generator script failed",
                        )
                    )
                    continue
            else:
                exec_name = info["source_file"]

            build_res = run_cmd(["make", f"-j{jobs}"], cwd=bench_dir, env=env)
            if build_res.returncode != 0:
                results.append(
                    TestResult(
                        suite="benchmarks/hw",
                        name=f"{bench}/{exec_name}",
                        status="FAIL",
                        duration_s=0.0,
                        returncode=build_res.returncode,
                        stdout=build_res.stdout,
                        stderr=build_res.stderr,
                        reason="Compilation failed",
                    )
                )
                continue

            exec_path = bench_dir / "bin" / exec_name

            # 1. Functionality check
            func_id = f"{bench}/{exec_name} [functionality -o {order}]"
            t0 = time.perf_counter()
            func_res = run_cmd(
                [str(exec_path), "-p", "tps", "-o", str(order), "-c"],
                cwd=bench_dir,
                timeout=timeout,
            )
            dt = time.perf_counter() - t0
            func_ok = any(
                line.startswith("# Functionality") and line.split()[-1] == "[OK]"
                for line in func_res.stdout.splitlines()
            )
            results.append(
                TestResult(
                    suite="benchmarks/hw",
                    name=func_id,
                    status="PASS" if func_ok else "FAIL",
                    duration_s=dt,
                    returncode=func_res.returncode,
                    stdout=func_res.stdout,
                    stderr=func_res.stderr,
                    reason="" if func_ok else "Functionality check failed",
                )
            )

            # 2. Security properties check (without and with glitches)
            for glitches in (False, True):
                glitch_key = "w/ g" if glitches else "no g"
                glitch_flag = "-g" if glitches else "-ng"
                for prop in sec_props:
                    test_id = f"{bench}/{exec_name} [-p {prop} -o {order} {glitch_flag}]"
                    expected_ok = info[glitch_key][prop]
                    cmd = [str(exec_path), "-p", prop, "-o", str(order), glitch_flag]

                    t0 = time.perf_counter()
                    try:
                        r = run_cmd(cmd, cwd=bench_dir, timeout=timeout)
                    except subprocess.TimeoutExpired:
                        dt = time.perf_counter() - t0
                        results.append(
                            TestResult(
                                suite="benchmarks/hw",
                                name=test_id,
                                status="TIMEOUT",
                                duration_s=dt,
                                returncode=-1,
                                stdout="",
                                stderr="",
                                reason=f"Timed out after {timeout}s",
                            )
                        )
                        continue

                    dt = time.perf_counter() - t0
                    lines = r.stdout.strip().splitlines()
                    verif_res = bool(lines and lines[-1].split()[-1] == "0")
                    result_ok = verif_res == expected_ok

                    if not result_ok and not verif_res:
                        t1 = time.perf_counter()
                        try:
                            r = run_cmd(cmd + ["-nfp"], cwd=bench_dir, timeout=timeout)
                            dt += time.perf_counter() - t1
                            lines = r.stdout.strip().splitlines()
                            verif_res = bool(lines and lines[-1].split()[-1] == "0")
                            result_ok = verif_res == expected_ok
                        except subprocess.TimeoutExpired:
                            dt += time.perf_counter() - t1
                            results.append(
                                TestResult(
                                    suite="benchmarks/hw",
                                    name=test_id + " (-nfp)",
                                    status="TIMEOUT",
                                    duration_s=dt,
                                    returncode=-1,
                                    stdout="",
                                    stderr="",
                                    reason=f"Timed out after {timeout}s",
                                )
                            )
                            continue

                    if r.returncode == 0 and result_ok:
                        results.append(
                            TestResult(
                                suite="benchmarks/hw",
                                name=test_id,
                                status="PASS",
                                duration_s=dt,
                                returncode=0,
                                stdout=r.stdout,
                                stderr=r.stderr,
                            )
                        )
                    else:
                        results.append(
                            TestResult(
                                suite="benchmarks/hw",
                                name=test_id,
                                status="FAIL",
                                duration_s=dt,
                                returncode=r.returncode,
                                stdout=r.stdout,
                                stderr=r.stderr,
                                reason=(
                                    f"Expected secure={expected_ok}, got secure={verif_res}"
                                    if r.returncode == 0
                                    else f"Exit code {r.returncode}"
                                ),
                            )
                        )
    return results


def main() -> int:
    parser = argparse.ArgumentParser(description="Global test runner for VerifMSI++")
    parser.add_argument(
        "-s",
        "--suites",
        nargs="+",
        default=UNIT_TEST_SUITES,
        help="Unit test suites to run (default: all suites in tests/)",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=os.cpu_count() or 4,
        help="Number of parallel workers for build and test execution",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=30.0,
        help="Timeout in seconds per test binary (default: 30s)",
    )
    parser.add_argument(
        "--no-build",
        action="store_true",
        help="Skip building the library and test binaries",
    )
    parser.add_argument(
        "--no-python",
        action="store_true",
        help="Skip running the Python bindings smoke test",
    )
    parser.add_argument(
        "-b",
        "--benchmarks",
        action="store_true",
        help="Also build and run the hardware benchmarks against dico_ref.py",
    )
    parser.add_argument(
        "--max-order",
        type=int,
        default=1,
        help="Maximum security order for hardware benchmarks when -b is used (default: 1)",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Treat tests without assertions (NO_ASSERT) as failures",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Show detailed build and test output",
    )
    args = parser.parse_args()

    start_time = time.perf_counter()

    if not args.no_build:
        if not build_library(args.jobs, args.verbose):
            return 1

    all_results: List[TestResult] = []
    build_failures: List[Tuple[str, str]] = []

    for suite in args.suites:
        suite_dir = ROOT_DIR / "tests" / suite
        if not suite_dir.is_dir():
            print(f"[WARN] Test suite directory not found: {suite_dir}")
            continue

        if not args.no_build:
            ok, err_msg = build_test_suite(suite, args.jobs, args.verbose)
            if not ok:
                print(f"[FAIL] Suite '{suite}' failed to compile:\n{err_msg}")
                build_failures.append((suite, err_msg))
                continue

        cpp_files = sorted(suite_dir.glob("*.cpp"))
        binaries = [suite_dir / "bin" / f.stem for f in cpp_files]
        missing = [b for b in binaries if not b.is_file()]
        if missing:
            for m in missing:
                all_results.append(
                    TestResult(
                        suite=suite,
                        name=m.name,
                        status="FAIL",
                        duration_s=0.0,
                        returncode=-1,
                        stdout="",
                        stderr="",
                        reason="Binary not found",
                    )
                )
            binaries = [b for b in binaries if b.is_file()]

        suite_results: List[TestResult] = []
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
            futures = [
                pool.submit(execute_single_binary, suite, b, args.timeout)
                for b in binaries
            ]
            for fut in concurrent.futures.as_completed(futures):
                suite_results.append(fut.result())

        suite_results.sort(key=lambda r: r.name)
        all_results.extend(suite_results)

        n_pass = sum(1 for r in suite_results if r.status == "PASS")
        n_fail = sum(1 for r in suite_results if r.status in ("FAIL", "TIMEOUT"))
        n_warn = sum(1 for r in suite_results if r.status == "NO_ASSERT")
        status_tag = "OK" if n_fail == 0 else "FAIL"
        warn_str = f", {n_warn} no-assert" if n_warn > 0 else ""
        print(
            f"  [{status_tag:4s}] Suite {suite:20s}: "
            f"{n_pass:3d}/{len(suite_results):3d} passed{warn_str}"
        )

        if args.verbose:
            for r in suite_results:
                print(f"         - {r.name:28s} [{r.status}] ({r.duration_s*1000:.1f} ms)")

    if not args.no_python:
        py_so = list(ROOT_DIR.glob("_core*.so"))
        if py_so:
            py_res = run_python_bindings_test(args.timeout)
            all_results.append(py_res)
            tag = "OK" if py_res.status == "PASS" else "FAIL"
            print(f"  [{tag:4s}] Suite {'python':20s}:   1/  1 passed" if tag == "OK" else f"  [FAIL] Suite python: {py_res.reason}")

    if args.benchmarks:
        hw_results = run_hw_benchmarks(
            args.jobs, args.max_order, args.timeout, args.verbose
        )
        all_results.extend(hw_results)
        n_pass = sum(1 for r in hw_results if r.status == "PASS")
        n_fail = sum(1 for r in hw_results if r.status != "PASS")
        tag = "OK" if n_fail == 0 else "FAIL"
        print(
            f"  [{tag:4s}] Suite {'benchmarks/hw':20s}: "
            f"{n_pass:3d}/{len(hw_results):3d} passed"
        )

    total_time = time.perf_counter() - start_time
    passed = [r for r in all_results if r.status == "PASS"]
    failed = [r for r in all_results if r.status in ("FAIL", "TIMEOUT")]
    no_assert = [r for r in all_results if r.status == "NO_ASSERT"]

    print("\n" + "=" * 70)
    print(
        f"Summary: {len(passed)} passed, {len(failed)} failed, "
        f"{len(no_assert)} without assertion (total: {len(all_results)} in {total_time:.2f}s)"
    )
    print("=" * 70)

    if no_assert:
        print("\nTests without OK/KO assertion (NO_ASSERT):")
        by_suite: Dict[str, List[str]] = {}
        for r in no_assert:
            by_suite.setdefault(r.suite, []).append(r.name)
        for s, names in by_suite.items():
            print(f"  - {s}: {', '.join(names)}")

    if build_failures:
        print("\nSuite compilation failures:")
        for s, msg in build_failures:
            print(f"  - {s}: {msg.splitlines()[-1] if msg.strip() else 'Build error'}")

    if failed:
        print("\nFailed tests:")
        for r in failed:
            print(f"  [FAIL] {r.suite}/{r.name}: {r.reason}")
            if r.stdout.strip():
                for line in r.stdout.strip().splitlines()[-5:]:
                    print(f"         stdout: {line}")
            if r.stderr.strip():
                for line in r.stderr.strip().splitlines()[-5:]:
                    print(f"         stderr: {line}")

    if build_failures or failed or (args.strict and no_assert):
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
