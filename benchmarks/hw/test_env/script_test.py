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
    print('\n')
    print('This script runs all the hw bechmarks and checks for their results for all the security property. In the output file, the \'✔\' and \'✘\' symbols indicate success and failure respectively, for the security check. If a security check fails while it is expected to succeed, the verification is run again using enumeration upon failure. In this case, the \'*\' mark is added in the result file. The \'OK\' and \'KO\' strings express the compliance with the expected verification result. Each verification is made at an order equal to the designed one (usually number of shares minus one).')
    print('Arguments:')
    print('<max_order>                    : Set the maximum security order to check for generic gadgets')
    print('Options:')
    print('-g,    --generation            : Generate files to test(default: %s)' % (generateFiles and 'Yes' or 'No'))
    print('-ng,   --no-generation         : Do not generate files to test(default: %s)' % (generateFiles and 'No' or 'Yes'))
    print('-h,    --help                  : Displays this help')



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



def withoutGliches(maxOrder, generateFiles):

    for bench in refResult:
        os.chdir(f"../{refResult[bench]['dir']}")
        res = ""

        for order in range(1, maxOrder + 1):

            if refResult[bench]["gen"]:
                refResult[bench]["source_file"] = f"{bench}_gen_{order + 1}_shares"
                if generateFiles :
                    subprocess.run(["python3", refResult[bench]["gen_file"], "-n", str(order + 1)])
                refResult[bench]["verif_order"] = order

            if order == refResult[bench]["verif_order"]:
                if "shares" in bench:
                    res += f"{bench} no glitches{SMALLSPACE}" #
                else:
                    res += f"{bench} {order + 1} shares no glitches " #



                nb_spaces = 44 - (len(bench) + 21)
                res += " " * nb_spaces

                subprocess.run(["make"])

                for p in range(0, len(secProps)):
                    (secure, resFunc) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-ng", True, True) #
                    secure += " "
                    same = checkResult(secure, bench, "no g", secProps[p]) #
                    if not same and secure[0] == "✘":
                        (secure, _) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-ng", False, False) #
                        secure += "*"
                    res += f"{secure}{HUGESPACE}"
     
                res += f"{resFunc}\n"


        with open(f"../test_env/{resultFile}", "a") as f:
            f.write(res)


    with open(f"../test_env/{resultFile}", "a") as f:
        f.write("\n")




def withGliches(maxOrder, generateFiles):

    for bench in refResult:
        os.chdir(f"../{refResult[bench]['dir']}")
        res = ""

        for order in range(1, maxOrder + 1):

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




def createCompFile(maxOrder):
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

            for i in range(1, maxOrder + 1):
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
        print('*** Error: first argument (max order) must be an integer', file = sys.stderr)
        usage(generateFiles)
        sys.exit(1)

    idx = 1
    maxOrder = None
    while idx < len(sys.argv):
        arg = sys.argv[idx]
        if arg == '-h' or arg == '--help':
            usage(generateFiles)
            sys.exit(0)
        elif arg == '-g' or arg == '--generation':
            generateFiles = True
        elif arg == '-ng' or arg == '--no-generation':
            generateFiles = False
        elif maxOrder == None:
            maxOrder = int(arg)
            idx += 1
        else:
            print('*** Error: unrecognized option: %s' % arg, file = sys.stderr)
            usage(generateFiles)
            sys.exit(1)
        idx += 1

    if maxOrder < 1:
        print("*** Error: max order must be greater than or equal to 1", file = sys.stderr)
        sys.exit(1)

    if os.path.exists(resultFile):
        os.remove(resultFile)

    # touch $resultFile + initialisation des en-têtes
    with open(resultFile, "w") as f:
        f.write(" " * 39)
        for j in range(0, len(secProps)):
            f.write(f"      {secProps[j]}{SMALLSPACE}")
        f.write("     check fonctionality\n\n")


    withoutGliches(maxOrder, generateFiles)
    withGliches(maxOrder, generateFiles)
    createCompFile(maxOrder)











