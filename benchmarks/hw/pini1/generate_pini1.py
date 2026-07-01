# Copyright (C) 2026, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Lucie Chauvière, Quentin L. Meunier

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from init_generator import Gadget
 


def generate_pini1(*argv):
    
    gadget = Gadget(prop = 'pini', benchmark = 'pini1', currentScript = os.path.basename(__file__), article = '[1] Gaëtan Cassiers and François-Xavier Standaert. Trivially and Efficiently Composing Masked Gadgets With Probe Isolating Non-Interference, in IEEE Transactions on Information Forensics and Security, vol. 15, pp. 2542-2555, 2020.', descriptionGenerator = '   This script generates a VerifMSI++ file of the PINI1 gadget from [1].')

    gadget.parseArgs()

    gadget.descriptionFile = "   This script contains a VerifMSI++ description of the PINI1 gadget from [1] with %d shares." % (gadget.nbShares)
    
    content = ''
    
    content = gadget.globalContent(content)

    content += '''


int32_t pini1_%d_shares(int32_t * nbCheck) {

''' % (gadget.nbShares)


    inputVars = ('x', 'y')
    outputVar = 'z'
    
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
    

    
    for i in range(gadget.nbShares):
        for j in range(i + 1, gadget.nbShares):
            content += '    Node & n_r%d%d = symbol(\"r%d%d\", \'M\', bitwidth);\n' % (i, j, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(i + 1, gadget.nbShares):
            content += '    HWElement & r%d%d = inputGate(n_r%d%d);\n' % (i, j, i, j)
            content += '    HWElement & r%d%d = inputGate(n_r%d%d);\n' % (j, i, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if (j != i):
                content += '    HWElement & s%d%d = xorGate(%s%d, r%d%d);\n' % (i, j, inputVars[1], j, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if (j != i):
                content += '    HWElement & p0_%d%d = genericGmul(notGate(%s%d), r%d%d);\n' % (i, j, inputVars[0], i, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if (j != i):
                content += '    HWElement & p1_%d%d = genericGmul(%s%d, s%d%d);\n' % (i, j, inputVars[0], i, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if (j != i):
                content += '    HWElement & z%d%d = xorGate(p0_%d%d, p1_%d%d);\n' % (i, j, i, j, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if(j != i):
                if(j == 0 or (i == 0 and j == 1)):
                    content += '    HWElement & sum%d%d = z%d%d;\n' % (i, j, i, j)
                elif(j == i+1):
                    content += '    HWElement & sum%d%d = xorGate(sum%d%d, z%d%d);\n' % (i, j, i, j - 2, i, j)
                else:
                    content += '    HWElement & sum%d%d = xorGate(sum%d%d, z%d%d);\n' % (i, j, i, j - 1, i, j)
                lastj = j 
        content += '    HWElement & sum%d = sum%d%d;\n' % (i, i, lastj)
    content += '\n'

    for i in range (gadget.nbShares):
        content += '    HWElement & %s%d%s%d = genericGmul(%s%d,%s%d);\n' % (inputVars[0], i, inputVars[1], i, inputVars[0], i, inputVars[1], i)

    for i in range(gadget.nbShares):
        content += '    HWElement & %s%d = xorGate(%s%d%s%d, sum%d);\n' % (outputVar, i, inputVars[0], i, inputVars[1], i, i)
    content += '\n'


    exps =  ' ^ '.join(['%s%d.getSymbExp()' % (outputVar, i) for i in range(gadget.nbShares)])
    exev =  '%s & %s' % (inputVars[0], inputVars[1])

    content = gadget.verifFunctionality(content, exps, exev)

    content += '    std::vector<HWElement *> outputs;\n'
    content += '\n'
    for i in range(gadget.nbShares):
        content += '    outputs.push_back(&%s%d);\n' % (outputVar, i)
    content += '\n'
    content += '    std::vector<std::vector<HWElement *>> outputList;\n'
    content += '    outputList.push_back(outputs);\n'

    content = gadget.check(content, inputVars, outputVar)

    content = gadget.initMain(content)

    f = open(gadget.outfile, 'w')
    f.write(content)
    f.close()



if __name__ == '__main__':
    generate_pini1(*sys.argv[1:])



