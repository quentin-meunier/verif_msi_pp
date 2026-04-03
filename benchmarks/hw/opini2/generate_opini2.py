# Copyright (C) 2026, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Lucie Chauvière, Quentin L. Meunier

import sys
import os
 
nbShares = 3
order = 2
prop = 'pini'
withGlitches = True
noFalsePositive = True
outfilePrefix = 'opini2_mult_gen'
outfile = None
currentScript = os.path.basename(__file__)
bitwidth = 1

article = '[1] Gaëtan Cassiers and François-Xavier Standaert. 2021. Provably Secure Hardware Masking in the Transition- and Glitch-Robust Probing Model: Better Safe than Sorry. IACR Trans. Cryptogr. Hardw. Embed. Syst. 2021, 2 (2021), 136–158. https://tches.iacr.org/index.php/TCHES/article/view/8790/8390'


def usage():
    print('Usage: %s [options]' % os.path.basename(__file__))
    print('   This script generates a VerifMSI++ file of the O-PINI2 gadget from [1].')
    print('Options:')
    print('-f,   --outfile <file>         : Set the name of the generated output file to <file> (default: %s)' % outfile)
    print('-n,   --nb-shares <n>          : Set the number of shares in the scheme to <n> (default: %d)' % nbShares)
    print('-o,   --order <n>              : Set the order of the verification to (default: %d)' % order)
    print('-p,   --prop                   : Set security property to verify: either \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference) \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) or \'tps\' (Treshold Probing Security). NI, SNI, RNI, PINI and OPINI use a share description for the inputs, while TPS uses a secrets + masks description (default: %s)' % prop)
    print('-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: %s)' % (withGlitches and 'Yes' or 'No'))
    print('-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: %s)' % (withGlitches and 'No' or 'Yes'))
    print('-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positive (defaut: %s)' % (noFalsePositive and 'No' or 'Yes'))
    print('-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: %s)' % (noFalsePositive and 'Yes' or 'No'))
    print('')
    print('%s' % article)


def propPy2cpp(prop):
    if prop == 'tps':
        return 'TPS'
    elif prop == 'ni':
        return 'NI'
    elif prop == 'sni':
        return 'SNI'
    elif prop == 'rni':
        return 'RNI'
    elif prop == 'pini':
        return 'PINI'
    elif prop == 'opini':
        return 'OPINI'
    assert(False)


def generate_opini2_mult(*argv):
    global nbShares
    global order
    global prop
    global withGlitches
    global noFalsePositive
    global outfile
    global bitwidth

    idx = 0
    while idx < len(argv):
        arg = argv[idx]
        if arg == '-h' or arg == '--help':
            usage()
            sys.exit(0)
        elif arg == '-f' or arg == '--outfile':
            idx += 1
            outfile = argv[idx]
        elif arg == '-n' or arg == '--nb-shares':
            idx += 1
            nbShares = int(argv[idx])
        elif arg == '-o' or arg == '--order':
            idx += 1
            order = int(argv[idx])
        elif arg == '-p' or arg == '--prop':
            idx += 1
            prop = argv[idx]
        elif arg == '-g' or arg == '--with-glitches':
            withGlitches = True
        elif arg == '-ng' or arg == '--without-glitches':
            withGlitches = False
        elif arg == '-fp' or arg == '--with-false-positive':
            noFalsePositive = False
        elif arg == '-nfp' or arg == '--without-false-positive':
            noFalsePositive = True
        else:
            print('*** Error: unrecognized option: %s' % arg, file = sys.stderr)
            usage()
            sys.exit(1)
        idx += 1
    
    if outfile == None:
        outfile = outfilePrefix + '_%d_shares.cpp' % nbShares
    
    if order >= nbShares:
        print('*** Error: the order of verification (%d) must be less than the number of shares (%d)' % (order, nbShares))
        sys.exit(1)
    
    if prop != 'ni' and prop != 'sni' and prop != 'tps' and prop != 'rni' and prop != 'pini' and prop != 'opini':
        print('*** Error: Unknown security property: %s' % prop)
        print('    Valid values are: \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference), \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) and \'tps\' (Treshold Probing Security)')
        sys.exit(1)
    
    
    
    nextRandNum = 0
    def getNewRandNum():
        nonlocal nextRandNum
        v = nextRandNum
        nextRandNum += 1
        return v
    
    
    content = '''/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * File generated by %s
 */

#include <cstring>

#include "verif_msi_pp.hpp"

''' % (currentScript)
    
    content += 'int32_t order = %d;\n' % order
    content += 'SecurityProperty secProp = %s;\n' % propPy2cpp(prop)
    content += 'bool withGlitches = %s;\n' % (withGlitches and 'true' or 'false')
    content += 'bool noFalsePositive = %s;\n' % (noFalsePositive and 'true' or 'false')
    content += 'bool dumpCirc = false;\n'
    content += 'bool checkFunctionality = false;\n'
    content += 'const char * circuitFilename = \"circuit.dot\";\n'
    content += 'int32_t bitwidth = %d;\n' % bitwidth
    content += '\n'
    
    content += '''void usage(const char * argv) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "   This script contains a VerifMSI++ description of the O-PINI2 gadget from [1] with %d shares." << std::endl;
    std::cout << "   This file was generated using the script %s" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-o,   --order <n>              : Set the order of the verification to (default: " << order << ")" << std::endl;
    std::cout << "-p,   --prop                   : Set security property to verify: either \\\'ni\\\' (Non-Interference), \\\'sni\\\' (Strong Non-Interference) \\\'rni\\\' (Relaxed Non-Interference), \\\'pini\\\' (Probe-Isolating Non-Interference), \\\'opini\\\' (Output-PINI) or \\\'tps\\\' (Treshold Probing Security). NI, SNI, RNI, PINI and OPINI use a share description for the inputs, while TPS uses a secrets + masks description (default: '" << secProp2str(secProp) << "')" << std::endl;
    std::cout << "-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: " << (withGlitches ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: " << (withGlitches ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positives (defaut: " << (noFalsePositive ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: " << (noFalsePositive ? "Yes" : "No") << ")" << std::endl;

    std::cout << "-d,   --dump-circuit           : Dump the circuit in dot format in a file named \\\"" << circuitFilename << "\\\" (default: " << (dumpCircuit ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-c,   --check-functionality    : Check the circuit functionality via exhaustive evaluation (default: " << (checkFunctionality ? "Yes" : "No") << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "%s" << std::endl;
}

    
std::vector<Node *> getShares(Node & s, int32_t nbShares) {
    if (secProp == TPS) {
        return getPseudoShares(s, nbShares);
    }
    else {
        return getRealShares(s, nbShares);
    }
}


int32_t opini2_mult_%d_shares(int32_t * nbCheck) {

''' % (nbShares, currentScript, article, nbShares)


    inputVars = ('x', 'y')
    outputVar = 'z'
    
    content += '    Node & %s = symbol(\"%s\", \'S\', bitwidth);\n' % (inputVars[0], inputVars[0])
    content += '    Node & %s = symbol(\"%s\", \'S\', bitwidth);\n' % (inputVars[1], inputVars[1])
    content += '\n'
    
    
    for var in inputVars:
        content += '    std::vector<Node *> v_%s = getShares(%s, %d);\n' % (var, var, nbShares)
    content += '\n'

    for var in inputVars:
        for i in range(1, nbShares + 1):
            content += '    Node & n_%s%d = *v_%s[%d];\n' % (var, i, var, i - 1)
        content += '\n'
    content += '\n'

    
    for var in inputVars:
        for sh in range(1, nbShares + 1):
            content += '    HWElement & %s%d = inputGate(n_%s%d);\n' % (var, sh, var, sh)
    content += '\n'
    

    
    for i in range(1, nbShares + 1):
        for j in range(i+1, nbShares + 1):
            content += '    Node & n_r%d_%d = symbol(\"r%d_%d\", \'M\', bitwidth);\n' % (i, j, i, j)
    content += '\n'

    for i in range(1, nbShares + 1):
        for j in range(i + 1, nbShares + 1):
            content += '    HWElement & r%d_%d = inputGate(n_r%d_%d);\n' % (i, j, i, j)
            content += '    HWElement & r%d_%d = inputGate(n_r%d_%d);\n' % (j, i, i, j)
    content += '\n'

    for i in range(1, nbShares + 1):
        for j in range(i + 1, nbShares + 1):
            content += '    HWElement & r_r%d_%d = Register(r%d_%d);\n' % (i, j, i, j)
            content += '    HWElement & r_r%d_%d = Register(r%d_%d);\n' % (j, i, j, i)
    content += '\n'


    content += '    //---------------HPC2 multiplication-------------------\n'

    for i in range(1, nbShares + 1):
        for j in range(1, nbShares + 1):
            if(j!=i):
                content += '    HWElement & u%d_%d = andGate(notGate(%s%d), r_r%d_%d);\n' % (i, j, inputVars[0], i, i, j)
    content += '\n'

    for i in range(1, nbShares + 1):
        for j in range(1, nbShares + 1):
            if(j!=i):
                content += '    HWElement & v%d_%d = xorGate(notGate(%s%d), r%d_%d);\n' % (i, j, inputVars[1], j, i, j)
    content += '\n'

    for i in range(1, nbShares + 1):
        for j in range(1, nbShares + 1):
            if(j!=i):
                content += '    HWElement & xor%d_%d = xorGate(Register(u%d_%d), Register(andGate(%s%d, Register(v%d_%d))));\n' % (i, j, i, j, inputVars[0], i, i, j)
                if(j==1 or (i==1 and j==2)):
                    content += '    HWElement & sumXor%d_%d = xor%d_%d;\n' % (i, j, i, j)
                elif(j==i+1):
                    content += '    HWElement & sumXor%d_%d = xorGate(sumXor%d_%d, xor%d_%d);\n' % (i, j, i, j-2, i, j)
                else:
                    content += '    HWElement & sumXor%d_%d = xorGate(sumXor%d_%d, xor%d_%d);\n' % (i, j, i, j-1, i, j)
                lastj = j 
        content += '    HWElement & bigXor%d = sumXor%d_%d;\n' % (i, i, lastj)
    content += '\n'

    for i in range(1, nbShares + 1):
        content += '    HWElement & c%d = xorGate(Register(andGate(%s%d, Register(%s%d))), bigXor%d);\n' % (i, inputVars[0], i, inputVars[1], i, i)
    content += '\n'

    content += '    //---------------OPINI multiplication------------------\n'

    for i in range(nbShares):
        content += '    HWElement & d%d = c%d;\n' % (i, i + 1)
    content += '\n'

    for i in range(nbShares - 1):
        content += '    Node & n_s%d = symbol(\"s%d\", \'M\', bitwidth);\n' % (i, i)
    content += '\n'

    for i in range(nbShares - 1):
        content += '    HWElement & s%d = inputGate(n_s%d);\n' % (i, i)

    content += '    HWElement & xorS0 = s0;\n'
    if(nbShares > 1):
        for i in range(1, nbShares - 1):
            content += '    HWElement & xorS%d = xorGate(xorS%d, s%d);\n' % (i, i - 1, i)
    content += '    HWElement & s%d = xorS%d;\n' % (nbShares - 1, nbShares - 2)
    content += '\n'

    for i in range(nbShares):
        content += '    HWElement & %s%d = Register(xorGate(d%d, Register(s%d)));\n' % (outputVar, i, i, i)
    content += '\n'

    content += '\n'
    content += '    if (checkFunctionality) {\n'
    content += '        bool res = compareExpsWithExev(' + ' ^ '.join(['%s%d.getSymbExp()' % (outputVar, i) for i in range(nbShares)]) + ', %s & %s);\n' % (inputVars[0], inputVars[1])
    content += '        if (res) {\n'
    content += '            std::cout << "# Functionality (exhaustive evaluation): [OK]" << std::endl;\n'
    content += '        }\n'
    content += '        else {\n'
    content += '            std::cout << "# Functionality (exhaustive evaluation): [KO]" << std::endl;\n'
    content += '        }\n'
    content += '    }\n'
    content += '\n'
    
    content += '    std::vector<HWElement *> outputs;\n'
    content += '\n'
    for i in range(nbShares):
        content += '    outputs.push_back(&%s%d);\n' % (outputVar, i)
    content += '\n'
    content += '    std::vector<std::vector<HWElement *>> outputList;\n'
    content += '    outputList.push_back(outputs);\n'
    
    content += '    if (dumpCirc) {\n'
    content += '        dumpCircuit(circuitFilename, outputs);\n'
    content += '    }\n'
    
    content += '    int32_t nbLeak = checkSecurity(order, withGlitches, secProp, outputs, noFalsePositive, nbCheck);\n'
    content += '    return nbLeak;\n'
    content += '}\n'
    content += '\n'
    content += '\n'
    
    content += '''int main(int argc, const char ** argv) {

    int32_t idx = 1;
    while (idx < argc) {
        const char * arg = argv[idx];
        if (strcmp(arg, "-h") == 0 or strcmp(arg, "--help") == 0) {
            usage(argv[0]);
            exit(0);
        }
        else if (strcmp(arg, "-o") == 0 or strcmp(arg, "--order") == 0) {
            idx += 1;
            order = atoi(argv[idx]);
        }
        else if (strcmp(arg, "-p") == 0 or strcmp(arg, "--prop") == 0) {
            idx += 1;
            const char * prop = argv[idx];
            if (strcmp(prop, "tps") == 0) {
                secProp = TPS;
            }
            else if (strcmp(prop, "ni") == 0) {
                secProp = NI;
            }
            else if (strcmp(prop, "sni") == 0) {
                secProp = SNI;
            }
            else if (strcmp(prop, "rni") == 0) {
                secProp =  RNI;
            }
            else if (strcmp(prop, "pini") == 0) {
                secProp = PINI;
            }
            else if (strcmp(prop, "opini") == 0) {
                secProp = OPINI;
            }
            else {
                std::cerr << "*** Error: Unknown security property: ";
                std::cerr << "    Valid values are: \\\'ni\\\' (Non-Interference), \\\'sni\\\' (Strong Non-Interference), \\\'rni\\\' (Relaxed Non-Interference), \\\'pini\\\' (Probe-Isolating Non-Interference), \\\'opini\\\' (Output-PINI) and \\\'tps\\\' (Treshold Probing Security)" << std::endl;
                exit(1);
            }
        }
        else if (strcmp(arg, "-g") == 0 or strcmp(arg, "--with-glitches") == 0) {
            withGlitches = true;
        }
        else if (strcmp(arg, "-ng") == 0 or strcmp(arg, "--without-glitches") == 0) {
            withGlitches = false;
        }
        else if (strcmp(arg, "-fp") == 0 or strcmp(arg, "--with-false-positive") == 0) {
            noFalsePositive = false;
        }
        else if (strcmp(arg, "-nfp") == 0 or strcmp(arg, "--without-false-positive") == 0) {
            noFalsePositive = true;
        }
        else if (strcmp(arg, "-d") == 0 or strcmp(arg, "--dump-circuit") == 0) {
            dumpCirc = true;
        }
        else if (strcmp(arg, "-c") == 0 or strcmp(arg, "--check-functionality") == 0) {
            checkFunctionality = true;
        }
        else {
            std::cerr << "*** Error: unrecognized option: " << arg << std::endl;
            usage(argv[0]);
            exit(1);
        }
        idx += 1;
    }
    
    
    if (order >= %d) {
        std::cerr << "*** Error: the order of verification (%d) must be less than the number of shares (%d)" << std::endl;
        exit(1);
    } 


    int32_t nbCheck;
    int32_t nbLeak = opini2_mult_%d_shares(&nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    return 0;
}

''' % (nbShares, nbShares, order, nbShares)

    f = open(outfile, 'w')
    f.write(content)
    f.close()


if __name__ == '__main__':
    generate_opini2_mult(*sys.argv[1:])



