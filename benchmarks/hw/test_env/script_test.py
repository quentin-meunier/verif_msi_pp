# Copyright (C) 2026, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Lucie Chauvière, Quentin L. Meunier

import sys
import os
import subprocess
import re
from dico_ref import refResult



SMALLSPACE = " " * 10
MEDSPACE = " " * 16
HUGESPACE = " " * 18

FILE = "result.txt"
CHECK_FONCTIONALITY = ""

PROP = ["tps ", "ni  ", "sni ", "rni ", "pini", "opini"]

# list of (bench, folder)
BENCHMARK_GEN = [("dom_and", "dom_and"), ("hpc3", "hpc3"), ("hpc4", "hpc4"), ("isw_and", "isw_and"), ("isw_and_refresh", "isw_and_refresh"), ("opini1_mult", "opini1"), ("opini2_mult", "opini2"), ("pini1", "pini1"), ("pini_mult", "pini_mult")]
BENCHMARK_NO_GEN = [("otsm", "otsm"), ("tsm", "tsm"), ("tsmp_2_inputs", "tsm_plus"), ("tsmp_3_inputs", "tsm_plus"), ("gms_and_3_shares", "gms_and"), ("gms_and_5_shares", "gms_and")]




def usage(generateFiles):
    print('Usage: script_test.py <max_order> [options]')
    print('    This script contains tests for the VerifMSI++ benchmarks.')
    print('Arguments:')
    print('<max_order>                    : Set the maximum security order to check')
    print('Options:')
    print('-g,   --generation             : Generate files to test(default: %s)' % (generateFiles and 'Yes' or 'No'))
    print('-ng,   --no-generation         : Do not generate files to test(default: %s)' % (generateFiles and 'No' or 'Yes'))
    



def writeLineHeader(line):
    check = re.split(r'    +', line)
    separator = check[1]
    tab = line.split(separator, 1)
    return tab[0]



def runSetup(bench, prop, order, glitches, firstTime):
    global CHECK_FONCTIONALITY
    res = ""

    if CHECK_FONCTIONALITY == "":
        cmd = [f"./bin/{bench}", "-p", prop.strip(), "-o", str(order), glitches, "-c"]
    elif firstTime:
        cmd = [f"./bin/{bench}", "-p", prop.strip(), "-o", str(order), glitches]
    else:
        cmd = [f"./bin/{bench}", "-p", prop.strip(), "-o", str(order), glitches, "-nfp"]

    res = subprocess.run(cmd, capture_output = True, text = True)

    lines = [line.strip() for line in res.stdout.splitlines() if line.strip()]
    if lines:
        words = lines[-1].split()
        if words:
            res = words[-1]

    for line in lines:
        if line[:15] == "# Functionality":
            words = line.split()
            if words[-1] == "[OK]":
                CHECK_FONCTIONALITY = "    ✔"
            else:
                CHECK_FONCTIONALITY = "    ✘"

    if (res == "0"):
        return "✔"
    else:
        return "✘"
    
    return ""



def checkResult(res, bench, g, p):
    if (res[0] != refResult[bench][g][p.strip()]):
        return False

    return True



def withoutGliches(max_order, existingFile):
    global CHECK_FONCTIONALITY

    for i in range(0, len(BENCHMARK_GEN)):
        os.chdir(f"../{BENCHMARK_GEN[i][1]}")

        for j in range(1, max_order + 1):
            res = ""
            order = j
            nbShares = order + 1

            if generateFiles:
                subprocess.run(["python3", f"generate_{BENCHMARK_GEN[i][1]}.py", "-n", str(nbShares)])

            res += f"{BENCHMARK_GEN[i][0]} {nbShares} shares no glitches"

            nb_spaces = 44 - (len(BENCHMARK_GEN[i][0]) + 21)
            res += " " * nb_spaces

            subprocess.run(["make"])

            for k in range(1, order + 1):
                for p in range(0, len(PROP)):
                    secure = runSetup(f"{BENCHMARK_GEN[i][0]}_gen_{nbShares}_shares", PROP[p], k, "-ng", True) + " "
                    same = checkResult(secure, BENCHMARK_GEN[i][0], "no g", PROP[p])
                    if not same and secure == "✘ ":
                        secure = runSetup(f"{BENCHMARK_GEN[i][0]}_gen_{nbShares}_shares", PROP[p], k, "-ng", False) + "*"
                    res += f"{secure}{HUGESPACE}"
     
            res += (max_order - j) * (len(PROP) - 1) * f"    {HUGESPACE}" + (max_order - j) * f"{SMALLSPACE}" + f"{CHECK_FONCTIONALITY}\n"


            with open(f"../test_env/{FILE}", "a") as f:
                f.write(res)
            CHECK_FONCTIONALITY = ""


    for i in range(0, len(BENCHMARK_NO_GEN)):
        res = ""
        os.chdir(f"../{BENCHMARK_NO_GEN[i][1]}")

        if BENCHMARK_NO_GEN[i][1] == "gms_and":
            res += f"{BENCHMARK_NO_GEN[i][0]} no glitches"
            res += MEDSPACE
        else:
            res += f"{BENCHMARK_NO_GEN[i][0]} 2 shares no glitches"
            nb_spaces = 44 - (len(BENCHMARK_NO_GEN[i][0]) + 21)
            res += " " * nb_spaces

        subprocess.run(["make"])
         
        for k in range(1, max_order + 1):
            if (k == 2 and BENCHMARK_NO_GEN[i][0] == "gms_and_5_shares"):
                for p in range(0, len(PROP)):
                    secure = runSetup(f"{BENCHMARK_NO_GEN[i][0]}", PROP[p], k, "-ng", True) + " "
                    same = checkResult(secure, BENCHMARK_NO_GEN[i][0], "no g", PROP[p])
                    if not same and secure == "✘ ":
                        secure = runSetup(f"{BENCHMARK_NO_GEN[i][0]}", PROP[p], k, "-ng", False) + "*"
                    res += f"{secure}{HUGESPACE}"

            elif (k == 1):
                for p in range(0, len(PROP)):
                    secure = runSetup(f"{BENCHMARK_NO_GEN[i][0]}", PROP[p], k, "-ng", True) + " "
                    same = checkResult(secure, BENCHMARK_NO_GEN[i][0], "no g", PROP[p])
                    if not same and secure == "✘ ":
                        secure = runSetup(f"{BENCHMARK_NO_GEN[i][0]}", PROP[p], k, "-ng", False) + "*"
                    res += f"{secure}{HUGESPACE}"

        if BENCHMARK_NO_GEN[i][0] != "gms_and_5_shares":
            res += (max_order - 1) * (len(PROP) - 1) * f"    {HUGESPACE}" + (max_order - 1) * f"{SMALLSPACE}" + f"{CHECK_FONCTIONALITY}\n"
        else:
            res += (max_order - 2) * (len(PROP) - 1) * f"    {HUGESPACE}" + (max_order - 2) * f"{SMALLSPACE}" + f"{CHECK_FONCTIONALITY}\n"
            
        with open(f"../test_env/{FILE}", "a") as f:
            f.write(res)
        CHECK_FONCTIONALITY = ""
    

    with open(f"../test_env/{FILE}", "a") as f:
        f.write("\n")
    
    existingFile = True
    
    
def withGliches(max_order, existingFile):
    global CHECK_FONCTIONALITY

    for i in range(0, len(BENCHMARK_GEN)):
        os.chdir(f"../{BENCHMARK_GEN[i][1]}")
        
        for j in range(1, max_order + 1):
            res = ""
            order = j
            nbShares = order + 1

            if generateFiles:
                subprocess.run(["python3", f"generate_{BENCHMARK_GEN[i][1]}.py", "-n", str(nbShares)])
            
            res = f"{BENCHMARK_GEN[i][0]} {nbShares} shares w/ glitches"

            nb_spaces = 44 - (len(BENCHMARK_GEN[i][0]) + 21)
            res += " " * nb_spaces

            subprocess.run(["make"])

            for k in range(1, order + 1):
                for p in range(0, len(PROP)):
                    secure = runSetup(f"{BENCHMARK_GEN[i][0]}_gen_{nbShares}_shares", PROP[p], k, "-g", True) + " "
                    same = checkResult(secure, BENCHMARK_GEN[i][0], "w/ g", PROP[p])
                    if not same and secure == "✘ ":
                        secure = runSetup(f"{BENCHMARK_GEN[i][0]}_gen_{nbShares}_shares", PROP[p], k, "-g", False) + "*"
                    res += f"{secure}{HUGESPACE}"

            res += (max_order - j) * (len(PROP) - 1) * f"    {HUGESPACE}" + (max_order - j) * f"{SMALLSPACE}" + f"{CHECK_FONCTIONALITY}\n"
            
            with open(f"../test_env/{FILE}", "a") as f:
                f.write(res)
            CHECK_FONCTIONALITY = ""
    
    
    for i in range(0, len(BENCHMARK_NO_GEN)):
        res = ""
        os.chdir(f"../{BENCHMARK_NO_GEN[i][1]}")

        if BENCHMARK_NO_GEN[i][1] == "gms_and":
            res += f"{BENCHMARK_NO_GEN[i][0]} w/ glitches"
            res += MEDSPACE
        else:
            res += f"{BENCHMARK_NO_GEN[i][0]} 2 shares w/ glitches"
            nb_spaces = 44 - (len(BENCHMARK_NO_GEN[i][0]) + 21)
            res += " " * nb_spaces

        subprocess.run(["make"])
         
        for k in range(1, max_order + 1):
            if (k == 2 and BENCHMARK_NO_GEN[i][0] == "gms_and_5_shares"):
                for p in range(0, len(PROP)):
                    secure = runSetup(f"{BENCHMARK_NO_GEN[i][0]}", PROP[p], k, "-g", True) + " "
                    same = checkResult(secure, BENCHMARK_NO_GEN[i][0], "w/ g", PROP[p])
                    if not same and secure == "✘  ":
                        secure = runSetup(f"{BENCHMARK_NO_GEN[i][0]}", PROP[p], k, "-g", False) + "*"
                    res += f"{secure}{HUGESPACE}"

            elif (k == 1):
                for p in range(0, len(PROP)):
                    secure = runSetup(f"{BENCHMARK_NO_GEN[i][0]}", PROP[p], k, "-g", True) + " "
                    same = checkResult(secure, BENCHMARK_NO_GEN[i][0], "w/ g", PROP[p])
                    if not same and secure == "✘ ":
                        secure = runSetup(f"{BENCHMARK_NO_GEN[i][0]}", PROP[p], k, "-g", False) + "*"
                    res += f"{secure}{HUGESPACE}"

        if BENCHMARK_NO_GEN[i][0] != "gms_and_5_shares":
            res += (max_order - 1) * (len(PROP) - 1) * f"    {HUGESPACE}" + (max_order - 1) * f"{SMALLSPACE}" + f"{CHECK_FONCTIONALITY}\n"
        else:
            res += (max_order - 2) * (len(PROP) - 1) * f"    {HUGESPACE}" + (max_order - 2) * f"{SMALLSPACE}" + f"{CHECK_FONCTIONALITY}\n"
            
        with open(f"../test_env/{FILE}", "a") as f:
            f.write(res)
        CHECK_FONCTIONALITY = ""


    with open(f"../test_env/{FILE}", "a") as f:
        f.write("\n")

    existingFile = True



def createCompFile(max_order):
    resultFile = open(f'../test_env/{FILE}', 'r')
    header = resultFile.readline()

    diffFile = open('../test_env/diff_result.txt', 'w')
    diffFile.write(header)
    
    for resultLine in resultFile:
        if resultLine == "\n":
            diffFile.write("\n")
        else:
            res = writeLineHeader(resultLine)
            tabResult = re.split(r'    +', resultLine)
            tabBench = tabResult[0].split()

            for i in range(0, len(tabResult) // 6):
                for p in range(0, len(PROP)):
                    bench = tabBench[0]
                    g = tabBench[-2] + " g"

                    res += tabResult[1 + (i * 6) + p]
                    if res[-1] != "*":
                        res += " "
                    if checkResult(tabResult[1 + (i * 6) + p], bench, g, PROP[p]):
                        res += f"(OK)  {SMALLSPACE}  "
                    else:
                        res += f"(KO)  {SMALLSPACE}  "

            for i in range(1, max_order + 1):
                if len(tabResult) == 2 + (len(PROP) * i):
                    res += f"{HUGESPACE}" * ((max_order - i) * len(PROP) -1)  + (max_order - i) * f" {SMALLSPACE}{HUGESPACE} " + tabResult[-1].strip()
                    if checkResult(tabResult[-1], bench, g, "check fonctionality"):
                        res += " (OK)"
                    else:
                        res += " (KO)"
                

            diffFile.write(res + "\n")

    resultFile.close()
    diffFile.close()




if __name__ == '__main__':

    generateFiles = True

    if len(sys.argv) < 2:
        print('*** Error: need argument <max_order>', file = sys.stderr)
        usage(generateFiles)
        sys.exit(1)

    if not sys.argv[1].isdigit():
        print('*** Error: first argument (max_order) must be an integer', file = sys.stderr)
        usage(generateFiles)
        sys.exit(1)

    max_order = int(sys.argv[1])

    idx = 0
    argv = sys.argv[2:]
    while idx < len(argv):
        arg = argv[idx]
        if arg == '-h' or arg == '--help':
            usage(generateFiles)
            sys.exit(0)
        elif arg == '-g' or arg == '--generation':
            generateFiles = True
        elif arg == '-ng' or arg == '--no-generation':
            generateFiles = False
        else:
            print('*** Error: unrecognized option: %s' % arg, file = sys.stderr)
            usage(generateFiles)
            sys.exit(1)
        idx += 1


    if max_order < 1:
        print("### Error: max_order must be greater or egal than 1")
        sys.exit(1)

    '''if os.path.exists(FILE):
        os.remove(FILE)

    # touch $FILE + initialisation des en-têtes
    with open(FILE, "w") as f:
        f.write(" " * 32)
        for i in range(1, max_order + 1):
            for j in range(0, len(PROP)):
                f.write(f"        {PROP[j]} order {i}")
        f.write("       check fonctionality\n\n")

    existingFile = True
    withoutGliches(max_order, generateFiles)
    withGliches(max_order, generateFiles)'''
    createCompFile(max_order)











