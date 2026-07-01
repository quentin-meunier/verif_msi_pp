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

resultFile = "result.txt"
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
        cmd = [f"./bin/{bench}", "-p", prop.strip(), "-o", str(order), glitches, "-c"]
        print(f"./bin/{bench} -p {prop} -o {order} {glitches} -c")
    elif firstTime:
        cmd = [f"./bin/{bench}", "-p", prop.strip(), "-o", str(order), glitches]
        print(f"./bin/{bench} -p {prop} -o {order} {glitches}")
    else:
        cmd = [f"./bin/{bench}", "-p", prop.strip(), "-o", str(order), glitches, "-nfp"]
        print(f"./bin/{bench} -p {prop} -o {order} {glitches} -nfp")

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
                resFunc = "✔"
            else:
                resFunc = "✘"

    if res == '0':
        return ("✔", resFunc)
    else:
        return ("✘", resFunc)
    


def checkResult(res, bench, g, p):
    return (res[0] == '✔' and refResult[bench][g][p.strip()]) or (res[0] == '✘' and not refResult[bench][g][p.strip()])



def executeProgramms(maxOrder, generateFiles):

    dicoRes = {}

    for bench in refResult:
        print(f"\ncd ../{refResult[bench]['dir']}")
        os.chdir(f"../{refResult[bench]['dir']}")
        res = ""
        
        dicoRes[bench] = {}

        for order in range(1, maxOrder + 1):

            if refResult[bench]["gen"]:
                refResult[bench]["source_file"] = f"{bench}_gen_{order + 1}_shares"
                if generateFiles :
                    subprocess.run(["python3", refResult[bench]["gen_file"], "-n", str(order + 1)])
                refResult[bench]["verif_order"] = order

            if order == refResult[bench]["verif_order"]:
                dicoRes[bench][order] = {}

                subprocess.run(["make"])

                dicoRes[bench][order]["no g"] = {}
                dicoRes[bench][order]["w/ g"] = {}
                for p in range(0, len(secProps)):
                    # without glitches
                    if p == 0:
                        (secure, resFunc) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-ng", True, True) #
                    else:
                        (secure, _) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-ng", True, False)
                    dicoRes[bench][order]["check functionality"] = resFunc
                    same = checkResult(secure, bench, "no g", secProps[p]) #
                    if not same and secure[0] == "✘":
                        (secure, _) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-ng", False, False) #
                        secure += "*"
                    dicoRes[bench][order]["no g"][secProps[p]] = secure


                    #with glitches
                    (secure, _) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-g", True, False) #
                    dicoRes[bench][order]["check functionality"] = resFunc
                    same = checkResult(secure, bench, "w/ g", secProps[p]) #
                    if not same and secure[0] == "✘":
                        (secure, _) = runSetup(f"{refResult[bench]['source_file']}", secProps[p], order, "-g", False, False) #
                        secure += "*"
                    dicoRes[bench][order]["w/ g"][secProps[p]] = secure


    return dicoRes




def createFiles(maxOrder, dicoRes):

    os.chdir("../test_env")

    header = " " * 39
    for j in range(0, len(secProps)):
        header += f"      {secProps[j]}{SMALLSPACE}"
    header += "     check fonctionality\n\n"


    contentResFile = header
    contentDiffFile = header


    glitches = ["no", "w/"]


    for g in glitches:
        for bench in dicoRes:
            for order in range(1, maxOrder + 1):
                if order in dicoRes[bench]:
                    if "shares" in bench:
                        contentResFile += f"{bench} {g} glitches{SMALLSPACE}"
                        contentDiffFile += f"{refResult[bench]['name']} {g} glitches{SMALLSPACE}"
                    else:
                        contentResFile += f"{bench} {order + 1} shares {g} glitches "
                        contentDiffFile += f"{refResult[bench]['name']} {order + 1} shares {g} glitches "

                    nb_spaces = 44 - (len(bench) + 21)
                    contentResFile += " " * nb_spaces
                    contentDiffFile += " " * nb_spaces


                    for p in secProps:

                        glitch = g + " g"
                        '''print("bench : ", bench)
                        print("order : ", order)
                        print("glitch : ", glitch)
                        print("prop : ", p)'''
                        res = f"{dicoRes[bench][order][glitch][p]}"
                        if res[-1] != "*":
                            res += " "

                        contentResFile += res + f"{HUGESPACE}"

                        contentDiffFile += res
            
                        if checkResult(dicoRes[bench][order][glitch][p], bench, glitch, p):
                            contentDiffFile += f"(OK)  {SMALLSPACE}  "
                        else:
                            contentDiffFile += f"(KO)  {SMALLSPACE}  "

                    res = f"     {dicoRes[bench][order]['check functionality']}"

                    contentResFile += res + "\n"

                    contentDiffFile += res
                    if dicoRes[bench][order]['check functionality'] == "✔":
                        contentDiffFile += " (OK)\n"
                    else:
                        contentDiffFile += " (KO)\n"

        contentResFile += "\n"
        contentDiffFile += "\n"


    with open(f'result.txt', 'w') as resFile:
        resFile.write(contentResFile)

    with open('diff_result.txt', 'w') as diffFile:
        diffFile.write(contentDiffFile)





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
        else:
            print('*** Error: unrecognized option: %s' % arg, file = sys.stderr)
            usage(generateFiles)
            sys.exit(1)
        idx += 1

    if maxOrder < 1:
        print("*** Error: max order must be greater than or equal to 1", file = sys.stderr)
        sys.exit(1)


    dicoRes = executeProgramms(maxOrder, generateFiles)
    createFiles(maxOrder, dicoRes)











