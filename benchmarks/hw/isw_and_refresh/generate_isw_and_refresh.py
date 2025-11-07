# Copyright (C) 2023, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Quentin L. Meunier

import sys
import os

# ISW AND with refresh of the shares of the first input
# (h_0, ..., h_{n-1}) gets circular_refresh(a_0, ..., a_{n-1})
# Then we apply ISW AND on (h_0, ..., h_{n-1}) and (b_0, ..., b_{n-1}).
# Circular refresh is defined as: h_i = (rr_i ^ rr_{i + 1 % n}) ^ ai

nbShares = 3
order = 2
prop = 'ni'
withGlitches = False
noFalsePositive = False
withAdditionalRand = True
outfilePrefix = 'isw_and_refresh_gen'
outfile = None


def usage():
    print('Usage: %s [options]' % os.path.basename(__file__))
    print('   This script generates a VerifMSI++ file describing a circuit implementing the logical AND following the ISW scheme with share refreshing from [1].')
    print('Options:')
    print('-f,   --outfile <file>         : Set the name of the generated output file to <file> (default: %s)' % outfile)
    print('-n,   --nb-shares <n>          : Set the number of shares in the scheme to <n> (default: %d)' % nbShares)
    print('-o,   --order <n>              : Set the order of the verification to (default: %d)' % order)
    print('-p,   --prop                   : Set security property to verify: either \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference) \'rni\' (Relaxed Non-Interference), \'pini\' (Probing-Isolating Non-Interference) or \'tps\' (Treshold Probing Security). NI, SNI, RNI and PINI use a share description for the inputs, while TPS uses a secrets + masks description (default: %s)' % prop)
    print('-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: %s)' % (withGlitches and 'Yes' or 'No'))
    print('-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: %s)' % (withGlitches and 'No' or 'Yes'))
    print('-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positive (defaut: %s)' % (noFalsePositive and 'No' or 'Yes'))
    print('-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: %s)' % (noFalsePositive and 'Yes' or 'No'))
    print('-r,   --with-rand              : Use an additional random for computing expressions of the form a_i & b_j,')
    print('-nr,  --with-rand              : Do not use additional random for computing expressions of the form a_i & b_j (default: %s)' % (withAdditionalRand and 'No' or 'Yes'))
    print('')
    print('[1] Ishai, Y., Sahai, A., & Wagner, D. (2003). Private circuits: Securing hardware against probing attacks. 23rd Annual International Cryptology Conference, 2003. Springer Berlin Heidelberg.')


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
    assert(False)


def generate_isw_and_refresh(*argv):
    global nbShares
    global order
    global prop
    global withGlitches
    global noFalsePositive
    global outfile
    global withAdditionalRandom

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
        elif arg == '-r' or arg == '--with-rand':
            withAdditionalRandom = True
        elif arg == '-nr' or arg == '--without-rand':
            withAdditionalRandom = False
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
    
    if prop != 'ni' and prop != 'sni' and prop != 'tps' and prop != 'rni' and prop != 'pini':
        print('*** Error: Unknown security property: %s' % prop)
        print('    Valid values are: \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference), \'rni\' (Relaxed Non-Interference), \'pini\' (Probing-Isolating Non-Interference) and \'tps\' (Treshold Probing Security)')
        sys.exit(1)
    
    
    
    nextRandNum = 0
    def getNewRandNum():
        nonlocal nextRandNum
        v = nextRandNum
        nextRandNum += 1
        return v
    
    
    content = '''/* Copyright (C) 2023, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * File generated by generate_isw_and_refresh.py
 */

#include <cstring>

#include "verif_msi_pp.hpp"

'''
    
    content += 'int32_t order = %d;\n' % order
    content += 'SecurityProperty secProp = %s;\n' % propPy2cpp(prop)
    content += 'bool withGlitches = %s;\n' % (withGlitches and 'true' or 'false')
    content += 'bool noFalsePositive = %s;\n' % (noFalsePositive and 'true' or 'false')
    content += 'bool dumpCirc = false;\n'
    content += 'bool checkFunctionality = false;\n'
    content += 'const char * circuitFilename = \"circuit.dot\";\n'
    content += '\n'
    
    content += '''void usage(const char * argv) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "   This script contains a VerifMSI++ description of a circuit implementing the logical AND following the DOM scheme from [1] with %d shares." << std::endl;
    std::cout << "   This file was generated using the script generate_isw_and.py" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-o,   --order <n>              : Set the order of the verification to (default: " << order << ")" << std::endl;
    std::cout << "-p,   --prop                   : Set security property to verify: either \\\'ni\\\' (Non-Interference), \\\'sni\\\' (Strong Non-Interference) \\\'rni\\\' (Relaxed Non-Interference), \\\'pini\\\' (Probing-Isolating Non-Interference) or \\\'tps\\\' (Treshold Probing Security). NI, SNI, RNI and PINI use a share description for the inputs, while TPS uses a secrets + masks description (default: '" << secProp2str(secProp) << "')" << std::endl;
    std::cout << "-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: " << (withGlitches ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: " << (withGlitches ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positives (defaut: " << (noFalsePositive ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: " << (noFalsePositive ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-d,   --dump-circuit           : Dump the circuit in dot format in a file named \\\"" << circuitFilename << "\\\" (default: " << (dumpCircuit ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-c,   --check-functionality    : Check the circuit functionality via exhaustive evaluation (default: " << (checkFunctionality ? "Yes" : "No") << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "[1] De Cnudde, T., Reparaz, O., Bilgin, B., Nikova, S., Nikov, V., & Rijmen, V. (2016). Masking AES with shares in hardware. In Cryptographic Hardware and Embedded Systems (CHES) 2016. Springer Berlin Heidelberg." << std::endl;
}

    
std::vector<Node *> getShares(Node & s, int32_t nbShares) {
    if (secProp == TPS) {
        return getPseudoShares(s, nbShares);
    }
    else {
        return getRealShares(s, nbShares);
    }
}


int32_t isw_and_refresh_%d_shares(int32_t * nbCheck) {

''' % (nbShares, nbShares)


    inputVars = ['a', 'b']
    outputVar = 'c'
    
    content += '    Node & %s = symbol(\"%s\", \'S\', 1);\n' % (inputVars[0], inputVars[0])
    content += '    Node & %s = symbol(\"%s\", \'S\', 1);\n' % (inputVars[1], inputVars[1])
    content += '\n'
    
    
    for var in inputVars:
        content += '    std::vector<Node *> v_%s = getShares(%s, %d);\n' % (var, var, nbShares)
    content += '\n'

    for var in inputVars:
        for i in range(nbShares):
            content += '    Node & n_%s%d = *v_%s[%d];\n' % (var, i, var, i)
        content += '\n'
    content += '\n'

    for i in range(nbShares):
        content += '    Node & n_rr%d = symbol(\"rr%d\", \'M\', 1);\n' % (i, i)
    content += '\n'
 

    for var in inputVars:
        for sh in range(nbShares):
            content += '    HWElement & %s%d = inputGate(n_%s%d);\n' % (var, sh, var, sh)
    content += '\n'
    
    for i in range(nbShares):
        content += '    HWElement & rr%d = inputGate(n_rr%d);\n' % (i, i)
    content += '\n'

    for i in range(nbShares):
        content += '    HWElement & hh%d = xorGate(rr%d, rr%d);\n' % (i, i, (i + 1) % nbShares)
    content += '\n'
    
    for i in range(nbShares):
        content += '    HWElement & h%d = xorGate(hh%d, %s%d);\n' % (i, i, inputVars[0], i)
    content += '\n'
 

    inputVars[0] = 'h'

    for i in range(nbShares - 1):
        for j in range(i + 1, nbShares):
            content += '    Node & n_z%d_%d = symbol(\"z%d_%d\", \'M\', 1);\n' % (i, j, i, j)
            content += '    HWElement & z%d_%d = inputGate(n_z%d_%d);\n' % (i, j, i, j)
            if withAdditionalRand:
                randNum = getNewRandNum()
                content += '    Node & n_r%d = symbol(\"r%d\", \'M\', 1);\n' % (randNum, randNum)
                content += '    HWElement & r%d = inputGate(n_r%d);\n' % (randNum, randNum)
                content += '    HWElement & %s%d%s%d = xorGate(andGate(xorGate(%s%d, r%d), %s%d), andGate(r%d, %s%d));\n' % (inputVars[0], i, inputVars[1], j, inputVars[0], i, randNum, inputVars[1], j, randNum, inputVars[1], j)
                randNum = getNewRandNum()
                content += '    Node & n_r%d = symbol(\"r%d\", \'M\', 1);\n' % (randNum, randNum)
                content += '    HWElement & r%d = inputGate(n_r%d);\n' % (randNum, randNum)
                content += '    HWElement & %s%d%s%d = xorGate(andGate(xorGate(%s%d, r%d), %s%d), andGate(r%d, %s%d));\n' % (inputVars[0], j, inputVars[1], i, inputVars[0], j, randNum, inputVars[1], i, randNum, inputVars[1], i)
            else:
                content += '    HWElement & %s%d%s%d = andGate(%s%d, %s%d);\n' % (inputVars[0], i, inputVars[1], j, inputVars[0], i, inputVars[1], j)
                content += '    HWElement & %s%d%s%d = andGate(%s%d, %s%d);\n' % (inputVars[0], j, inputVars[1], i, inputVars[0], j, inputVars[1], i)
            content += '    HWElement & z%d_%d = xorGate(xorGate(z%d_%d, %s%d%s%d), %s%d%s%d);\n' % (j, i, i, j, inputVars[0], i, inputVars[1], j, inputVars[0], j, inputVars[1], i)
    content += '\n'
    content += '\n'
    
    
    for i in range(nbShares):
        content += '    HWElement & %s%d_0 = andGate(%s%d, %s%d);\n' % (outputVar, i, inputVars[0], i, inputVars[1], i)
        idx = 1
        for j in range(nbShares):
            if i != j:
                if i != nbShares - 1 and j == nbShares - 1 or i == nbShares - 1 and j == nbShares - 2:
                    content += '    HWElement & %s%d = xorGate(%s%d_%d, z%d_%d);\n' % (outputVar, i, outputVar, i, (idx - 1), i, j)
                else:
                    content += '    HWElement & %s%d_%d = xorGate(%s%d_%d, z%d_%d);\n' % (outputVar, i, idx, outputVar, i, (idx - 1), i, j)
                idx += 1
        content += '\n'
    

    
    inputVars[0] = 'a'
    
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
    
    content += '    std::vector<HWElement *> outputs; // only c shares for isw_and_refresh\n'
    content += '\n'
    for i in range(nbShares):
        content += '    outputs.push_back(&c%d);\n' % i
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
            else {
                std::cerr << "*** Error: Unknown security property: ";
                std::cerr << "    Valid values are: \\\'ni\\\' (Non-Interference), \\\'sni\\\' (Strong Non-Interference), \\\'rni\\\' (Relaxed Non-Interference), \\\'pini\\\' (Probing-Isolating Non-Interference) and \\\'tps\\\' (Treshold Probing Security)" << std::endl;
                exit(1);
            }
        }
        else if (strcmp(arg, "-g") == 0 or strcmp(arg, "--with-glitches") == 0) {
            withGlitches = true;
        }
        else if (strcmp(arg, "-ng") == 0 or strcmp(arg, "--without-glitches") == 0) {
            withGlitches = false;
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
    int32_t nbLeak = isw_and_refresh_%d_shares(&nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    return 0;
}

''' % (nbShares, nbShares, order, nbShares)

    f = open(outfile, 'w')
    f.write(content)
    f.close()


if __name__ == '__main__':
    generate_isw_and_refresh(*sys.argv[1:])




