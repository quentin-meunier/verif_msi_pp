# Copyright (C) 2023, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Quentin L. Meunier

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from init_generator import Gadget



def generate_isw_and(*argv):

    gadget = Gadget(prop = 'ni', benchmark = 'isw_and', currentScript = os.path.basename(__file__), article = '[1] Ishai, Y., Sahai, A., & Wagner, D. (2003). Private circuits: Securing hardware against probing attacks. 23rd Annual International Cryptology Conference, 2003. Springer Berlin Heidelberg.', description_generator = '   This script generates a VerifMSI++ file describing a circuit implementing the logical AND following the ISW scheme from [1].')

    gadget.description_file = "   This script contains a VerifMSI++ description of a circuit implementing the logical AND following the ISW scheme from [1] with %d shares." % (gadget.nbShares)

    gadget.parseArgs()

    nextRandNum = 0
    def getNewRandNum():
        nonlocal nextRandNum
        v = nextRandNum
        nextRandNum += 1
        return v
    
    content = ''
    
    content = gadget.globalContent(content)
    
    content += '''

int32_t isw_and_%d_shares(int32_t * nbCheck) {

''' % (gadget.nbShares)
    

    inputVars = ('a', 'b')
    outputVar = 'c'
    
    content += '    Node & %s = symbol(\"%s\", \'S\', bitwidth);\n' % (inputVars[0], inputVars[0])
    content += '    Node & %s = symbol(\"%s\", \'S\', bitwidth);\n' % (inputVars[1], inputVars[1])
    content += '\n'
    
    
    for var in inputVars:
        content += '    std::vector<Node *> v_%s = getShares(%s, %d);\n' % (var, var, gadget.nbShares)
    content += '\n'

    for var in inputVars:
        for i in range(gadget.nbShares):
            content += '    Node & n_%s%d = *v_%s[%d];\n' % (var, i, var, i)
        content += '\n'
    content += '\n'

    
    for var in inputVars:
        for sh in range(gadget.nbShares):
            content += '    HWElement & %s%d = inputGate(n_%s%d);\n' % (var, sh, var, sh)
    content += '\n'
    

    for i in range(gadget.nbShares - 1):
        for j in range(i + 1, gadget.nbShares):
            content += '    Node & n_z%d_%d = symbol(\"z%d_%d\", \'M\', bitwidth);\n' % (i, j, i, j)
            content += '    HWElement & z%d_%d = inputGate(n_z%d_%d);\n' % (i, j, i, j)
            if gadget.withAdditionalRand:
                randNum = getNewRandNum()
                content += '    Node & n_r%d = symbol(\"r%d\", \'M\', bitwidth);\n' % (randNum, randNum)
                content += '    HWElement & r%d = inputGate(n_r%d);\n' % (randNum, randNum)
                content += '    HWElement & %s%d%s%d = xorGate(genericGmul(xorGate(%s%d, r%d), %s%d), genericGmul(r%d, %s%d));\n' % (inputVars[0], i, inputVars[1], j, inputVars[0], i, randNum, inputVars[1], j, randNum, inputVars[1], j)
                randNum = getNewRandNum()
                content += '    Node & n_r%d = symbol(\"r%d\", \'M\', bitwidth);\n' % (randNum, randNum)
                content += '    HWElement & r%d = inputGate(n_r%d);\n' % (randNum, randNum)
                content += '    HWElement & %s%d%s%d = xorGate(genericGmul(xorGate(%s%d, r%d), %s%d), genericGmul(r%d, %s%d));\n' % (inputVars[0], j, inputVars[1], i, inputVars[0], j, randNum, inputVars[1], i, randNum, inputVars[1], i)
            else:
                content += '    HWElement & %s%d%s%d = genericGmul(%s%d, %s%d);\n' % (inputVars[0], i, inputVars[1], j, inputVars[0], i, inputVars[1], j)
                content += '    HWElement & %s%d%s%d = genericGmul(%s%d, %s%d);\n' % (inputVars[0], j, inputVars[1], i, inputVars[0], j, inputVars[1], i)
            content += '    HWElement & z%d_%d = xorGate(xorGate(z%d_%d, %s%d%s%d), %s%d%s%d);\n' % (j, i, i, j, inputVars[0], i, inputVars[1], j, inputVars[0], j, inputVars[1], i)
    content += '\n'
    content += '\n'
    
    
    for i in range(gadget.nbShares):
        content += '    HWElement & %s%d_0 = genericGmul(%s%d, %s%d);\n' % (outputVar, i, inputVars[0], i, inputVars[1], i)
        idx = 1
        for j in range(gadget.nbShares):
            if i != j:
                if i != gadget.nbShares - 1 and j == gadget.nbShares - 1 or i == gadget.nbShares - 1 and j == gadget.nbShares - 2:
                    content += '    HWElement & %s%d = xorGate(%s%d_%d, z%d_%d);\n' % (outputVar, i, outputVar, i, (idx - 1), i, j)
                else:
                    content += '    HWElement & %s%d_%d = xorGate(%s%d_%d, z%d_%d);\n' % (outputVar, i, idx, outputVar, i, (idx - 1), i, j)
                idx += 1
        content += '\n'



    exps =  ' ^ '.join(['%s%d.getSymbExp()' % (outputVar, i) for i in range(gadget.nbShares)])
    exev =  '%s & %s' % (inputVars[0], inputVars[1])

    content = gadget.verifFunctionality(content, exps, exev)
    
    
    content += '    std::vector<HWElement *> outputs; // only c shares for isw_and\n'
    content += '\n'
    for i in range(gadget.nbShares):
        content += '    outputs.push_back(&c%d);\n' % i
    content += '\n'
    content += '    std::vector<std::vector<HWElement *>> outputList;\n'
    content += '    outputList.push_back(outputs);\n'
    content += '\n'

    content = gadget.check(content, inputVars, outputVar)

    content = gadget.initMain(content)

    f = open(gadget.outfile, 'w')
    f.write(content)
    f.close()


if __name__ == '__main__':
    generate_isw_and(*sys.argv[1:])




