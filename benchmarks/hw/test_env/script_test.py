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

colWidth = 16
firstColWidth = 39

resultFile = 'results.txt'
diffFile = 'diff.txt'
checkFunctionality = True

secProps = ['tps', 'ni', 'sni', 'rni', 'pini', 'opini']




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



def print_and_execute(cmd):
    print(' '.join(cmd))
    res = subprocess.run(cmd, capture_output = True, text = True)
    return res.stdout.splitlines()


def myChdir(d):
    print(f"cd {d}")
    os.chdir(d)


def writeToFile(filename, content):
    with open(filename, 'w') as f:
        f.write(content)



def runFunctionalityCheck(bench, order):
    cmd = [f"./bin/{bench}", '-p', secProps[0], '-o', str(order), '-c']
    res = print_and_execute(cmd)

    for line in res:
        if line[:15] == '# Functionality':
            words = line.split()
            return words[-1] == "[OK]"




def runVerif(bench, prop, order, glitches, noFalsePositive):

    cmd = [f"./bin/{bench}", '-p', prop, '-o', str(order), glitches]
    if noFalsePositive:
        cmd.append('-nfp')

    lines = print_and_execute(cmd)

    res = lines[-1].split()[-1]

    return res == '0'




def verifPrograms(maxOrder, generateFiles):

    dicoRes = {}


    for bench in refResult:
        myChdir(f"{refResult[bench]['dir']}")
        
        dicoRes[bench] = {}

        for order in range(1, maxOrder + 1):

            if refResult[bench]['gen'] or order == refResult[bench]['verif_order']:
                if refResult[bench]['gen']:
                    execFile = f"{bench}_gen_{order + 1}_shares"
                    if generateFiles:
                        cmd = ['python3', refResult[bench]['gen_file'], '-n', str(order + 1)]
                        print_and_execute(cmd)
                else:
                    execFile = refResult[bench]['source_file']

                print_and_execute(['make'])


                dicoRes[bench][order] = {}

                # Checking functionality
                dicoRes[bench][order]['func'] = runFunctionalityCheck(execFile, order)

                for glitches in (False, True):
                    glitch_str = glitches and 'w/ g' or 'no g'
                    glitch_param = glitches and '-g' or '-ng'
                    dicoRes[bench][order][glitch_str] = {}

                    for p in range(0, len(secProps)):
                        dicoRes[bench][order][glitch_str][secProps[p]] = {}

                        verifResult = runVerif(execFile, secProps[p], order, glitch_param, False)
                        dicoRes[bench][order][glitch_str][secProps[p]]['result'] = verifResult

                        resultOk = (verifResult == refResult[bench][glitch_str][secProps[p]])
                        dicoRes[bench][order][glitch_str][secProps[p]]['ok'] = resultOk

                        if not resultOk and not verifResult:
                            verifResult = runVerif(execFile, secProps[p], order, glitch_param, True)
                            dicoRes[bench][order][glitch_str][secProps[p]]['result'] = verifResult

                            resultOk = (verifResult == refResult[bench][glitch_str][secProps[p]])
                            dicoRes[bench][order][glitch_str][secProps[p]]['ok'] = resultOk

                            dicoRes[bench][order][glitch_str][secProps[p]]['used_enum'] = True
                        else:
                            dicoRes[bench][order][glitch_str][secProps[p]]['used_enum'] = False

        myChdir("..")
    return dicoRes




def createResultFiles(dicoRes):

    header = ' ' * firstColWidth
    for j in range(0, len(secProps)):
        header += secProps[j].upper() + ' ' * (colWidth - len(secProps[j]))
    header += 'Functionality\n\n'

    resFileContent = header
    diffFileContent = header

    glitches = ['no', 'w/']

    for g in glitches:
        for bench in dicoRes:
            for order in dicoRes[bench]:
                if refResult[bench]['gen']:
                    res = refResult[bench]['name'] + ' %d shares %s glitches' % (order + 1, g)
                else:
                    res = refResult[bench]['name'] + ' %s glitches' % g

                res += ' ' * (firstColWidth - len(res))
                resFileContent += res
                diffFileContent += res

                for p in secProps:

                    glitch = g + ' g'
                    res = ''
                    if dicoRes[bench][order][glitch][p]['result']:
                        res += '✔'
                    else:
                        res += '✘'
                    if dicoRes[bench][order][glitch][p]['used_enum']:
                        res += '*'
                    else:
                        res += ' '

                    resFileContent += res + ' ' * (colWidth - len(res))

                    if dicoRes[bench][order][glitch][p]['ok']:
                        res += '(OK)'
                    else:
                        res += '(KO)'

                    diffFileContent += res + ' ' * (colWidth - len(res))

                if dicoRes[bench][order]['func']:
                    res = '✔'
                else:
                    res = '✘'

                resFileContent += res + '\n'
                diffFileContent += res + '\n'

        resFileContent += '\n'
        diffFileContent += '\n'

    writeToFile(resultFile, resFileContent)
    writeToFile(diffFile, diffFileContent)





if __name__ == '__main__':

    scriptDir = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))
    benchsDir = os.path.abspath(os.path.join(scriptDir, '..'))

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
        print('*** Error: max order must be greater than or equal to 1', file = sys.stderr)
        sys.exit(1)


    myChdir(benchsDir)
    dicoRes = verifPrograms(maxOrder, generateFiles)
    myChdir(scriptDir)
    createResultFiles(dicoRes)











