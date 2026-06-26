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

resultFile = "test.txt"
checkFunctionality = True

secProps = ["tps ", "ni  ", "sni ", "rni ", "pini", "opini"]




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



def runSetup(bench, prop, order, glitches, firstTime, checkFunctionality):

    if checkFunctionality:
        print("bench : ", bench)
        print("order : ", order)
        print()
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

    resFunc = None
    for line in lines:
        if line[:15] == "# Functionality":
            words = line.split()
            if words[-1] == "[OK]":
                resFunc = "    ✔"
            else:
                resFunc = "    ✘"

    if res == '0':
        return ("✔", resFunc)
    else:
        return ("✘", resFunc)
    


def checkResult(res, bench, g, p):
    return (res[0] == '✔' and refResult[bench][g][p.strip()]) or (res[0] == '✘' and not refResult[bench][g][p.strip()])



def withoutGliches(max_order, generateFiles):

    for bench in refResult:
        os.chdir(f"../{refResult[bench]['dir']}")
        res = ""

        for order in range(1, max_order + 1):

            if refResult[bench]["gen"]:
                refResult[bench]["source_file"] = f"{bench}_gen_{order + 1}_shares"
                if generateFiles :
                    subprocess.run(["python3", refResult[bench]["gen_file"], "-n", str(order + 1)])
                refResult[bench]["verif_order"] = order

            if order == refResult[bench]["verif_order"]:
                if "shares" in bench:
                    res += f"{bench} no glitches{SMALLSPACE}"
                else:
                    res += f"{bench} {order + 1} shares no glitches "



                nb_spaces = 44 - (len(bench) + 21)
                res += " " * nb_spaces

                subprocess.run(["make"])

                for p in range(0, len(secProps)):
                    (secure, resFunc) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-ng", True, True)
                    secure += " "
                    same = checkResult(secure, bench, "no g", secProps[p])
                    if not same and secure[0] == "✘":
                        (secure, _) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-ng", False, False)
                        secure += "*"
                    res += f"{secure}{HUGESPACE}"
     
                res += f"{resFunc}\n"


        with open(f"../test_env/{resultFile}", "a") as f:
            f.write(res)


    with open(f"../test_env/{resultFile}", "a") as f:
        f.write("\n")




    
def withGliches(max_order, generateFiles):

    for bench in refResult:
        os.chdir(f"../{refResult[bench]['dir']}")
        res = ""

        for order in range(1, max_order + 1):

            if refResult[bench]["gen"]:
                refResult[bench]["source_file"] = f"{bench}_gen_{order + 1}_shares"
                if generateFiles :
                    subprocess.run(["python3", refResult[bench]["gen_file"], "-n", str(order + 1)])
                refResult[bench]["verif_order"] = order

            if order == refResult[bench]["verif_order"]:
                if "shares" in bench:
                    res += f"{bench} w/ glitches{SMALLSPACE}"
                else:
                    res += f"{bench} {order + 1} shares w/ glitches "



                nb_spaces = 44 - (len(bench) + 21)
                res += " " * nb_spaces

                subprocess.run(["make"])

                for p in range(0, len(secProps)):
                    (secure, resFunc) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-g", True, True)
                    secure += " "
                    same = checkResult(secure, bench, "w/ g", secProps[p])
                    if not same and secure[0] == "✘":
                        (secure, _) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-g", False, False)
                        secure += "*"
                    res += f"{secure}{HUGESPACE}"
     
                res += f"{resFunc}\n"


        with open(f"../test_env/{resultFile}", "a") as f:
            f.write(res)


    with open(f"../test_env/{resultFile}", "a") as f:
        f.write("\n")




def createCompFile(max_order):
    global resultFile

    resFile = open(f'../test_env/{resultFile}', 'r')
    header = resFile.readline()

    diffFile = open('../test_env/diff_result.txt', 'w')
    diffFile.write(header)
    
    for resultLine in resFile:
        if resultLine == "\n":
            diffFile.write("\n")
        else:
            res = writeLineHeader(resultLine)
            tabResult = re.split(r'    +', resultLine)
            tabBench = tabResult[0].split()

            for i in range(0, len(tabResult) // 6):
                for p in range(0, len(secProps)):
                    bench = tabBench[0]
                    g = tabBench[-2] + " g"

                    res += tabResult[1 + (i * 6) + p]
                    if res[-1] != "*":
                        res += " "
                    if checkResult(tabResult[1 + (i * 6) + p], bench, g, secProps[p]):
                        res += f"(OK)  {SMALLSPACE}  "
                    else:
                        res += f"(KO)  {SMALLSPACE}  "

            for i in range(1, max_order + 1):
                if len(tabResult) == 2 + (len(secProps) * i):
                    res += "     " + tabResult[-1].strip()
                    if tabResult[-1][0] == "✔":
                        res += " (OK)"
                    else:
                        res += " (KO)"
                

            diffFile.write(res + "\n")

    resFile.close()
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

    if os.path.exists(resultFile):
        os.remove(resultFile)

    # touch $resultFile + initialisation des en-têtes
    with open(resultFile, "w") as f:
        f.write(" " * 39)
        for j in range(0, len(secProps)):
            f.write(f"      {secProps[j]}{SMALLSPACE}")
        f.write("     check fonctionality\n\n")


    #createBenchList(generateFiles)
    withoutGliches(max_order, generateFiles)
    withGliches(max_order, generateFiles)
    createCompFile(max_order)











