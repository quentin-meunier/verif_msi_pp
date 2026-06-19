# Copyright (C) 2026, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Lucie Chauvière, Quentin L. Meunier

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from init_generator import Gadget





def generate_hpc3(*argv):

    gadget = Gadget(order = 1, prop = 'pini', benchmark = 'hpc3', currentScript = os.path.basename(__file__), article = '[1] D. Knichel, and A. Moradi. Low-latency hardware private circuits. Proceedings of the 2022 ACM SIGSAC Conference on Computer and Communications Security. 2022. https://eprint.iacr.org/2022/507', descriptionGenerator = '   This script generates a VerifMSI++ file describing a circuit implementing the HPC3 gadget from [1].')

    gadget.descriptionFile = "   This script contains a VerifMSI++ description of a circuit implementing the HPC3 gadget from [1] with %d shares." % (gadget.nbShares)

    gadget.parseArgs()
    
    content = ''
    
    content = gadget.globalContent(content)

    content += '''

int32_t hpc3_%d_shares(int32_t * nbCheck) {

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
        for j in range(i+1, gadget.nbShares):
            content += '    Node & n_r%d_%d = symbol(\"r%d_%d\", \'M\', bitwidth);\n' % (i, j, i, j)
            content += '    Node & n_r%d_%d_p = symbol(\"r%d_%d_p\", \'M\', bitwidth);\n' % (i, j, i, j)
    content += '\n'


    for i in range(gadget.nbShares):
        for j in range(i+1, gadget.nbShares):
            content += '    HWElement & r%d_%d = inputGate(n_r%d_%d);\n' % (j, i, i, j)
            content += '    HWElement & r%d_%d = inputGate(n_r%d_%d);\n' % (i, j, i, j)
            content += '    HWElement & r%d_%d_p = inputGate(n_r%d_%d_p);\n' % (j, i, i, j)
            content += '    HWElement & r%d_%d_p = inputGate(n_r%d_%d_p);\n' % (i, j, i, j)
    content += '    HWElement & cst1 = inputGate(constant(1, bitwidth));\n'
    content += '\n'



    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if(j!=i):
                content += '    HWElement & u%d_%d = Register(xorGate(%s%d, r%d_%d));\n' % (i, j, inputVars[1], j, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if(j!=i):
                content += '    HWElement & v%d_%d = Register(xorGate(genericGmul(xorGate(%s%d, cst1), r%d_%d), r%d_%d_p));\n' % (i, j, inputVars[0], i, i, j, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if(j!=i):
                content += '    HWElement & w%d_%d = xorGate(genericGmul(Register(%s%d), u%d_%d), v%d_%d);\n' % (i, j, inputVars[0], i, i, j, i, j)
    content += '\n'

    for i in range(gadget.nbShares):
        content += '    HWElement & r_%s%d%s%d = Register(genericGmul(%s%d, %s%d));\n' % (inputVars[0], i,  inputVars[1], i, inputVars[0], i, inputVars[1], i)
    content += '\n'


    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            if(j!=i):
                if(j==0 or (i==0 and j==1)):
                    content += '    HWElement & sum%d_%d = w%d_%d;\n' % (i, j, i, j)
                elif(j==i+1):
                    content += '    HWElement & sum%d_%d = xorGate(sum%d_%d, w%d_%d);\n' % (i, j, i, j-2, i, j)
                else:
                    content += '    HWElement & sum%d_%d = xorGate(sum%d_%d, w%d_%d);\n' % (i, j, i, j-1, i, j)
                lastj = j 
        content += '    HWElement & sum%d = sum%d_%d;\n' % (i, i, lastj)
    content += '\n'



    for i in range(gadget.nbShares):
        content += '    HWElement & %s%d = xorGate(r_%s%d%s%d, sum%d);\n' % (outputVar, i, inputVars[0], i, inputVars[1], i, i)
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
    generate_hpc3(*sys.argv[1:])



