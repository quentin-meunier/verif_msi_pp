# Copyright (C) 2026, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Lucie Chauvière, Quentin L. Meunier

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from init_generator import Gadget





def generate_hpc2(*argv):

    gadget = Gadget(prop = 'opini', benchmark = 'hpc2', currentScript = os.path.basename(__file__), article = '[1] Gaëtan Cassiers and François-Xavier Standaert. 2021. Provably Secure Hardware Masking in the Transition- and Glitch-Robust Probing Model: Better Safe than Sorry. IACR Trans. Cryptogr. Hardw. Embed. Syst. 2021, 2 (2021), 136–158. https://tches.iacr.org/index.php/TCHES/article/view/8790/8390', descriptionGenerator = '   This script generates a VerifMSI++ file of the HPC2 gadget from [1].')

    gadget.descriptionFile = "   This script contains a VerifMSI++ description of the HPC2 gadget from [1] with %d shares." % (gadget.nbShares)

    gadget.parseArgs()
    
    content = ''
    
    content = gadget.globalContent(content)

    content += '''

int32_t hpc2_%d_shares(int32_t * nbCheck) {

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
        for i in range(1, gadget.nbShares + 1):
            content += '    Node & n_%s%d = *v_%s[%d];\n' % (var, i, var, i - 1)
        content += '\n'
    content += '\n'

    
    for var in inputVars:
        for sh in range(1, gadget.nbShares + 1):
            content += '    HWElement & %s%d = inputGate(n_%s%d);\n' % (var, sh, var, sh)
    content += '\n'


    
    for i in range(1, gadget.nbShares + 1):
        for j in range(i+1, gadget.nbShares + 1):
            content += '    Node & n_r%d_%d = symbol(\"r%d_%d\", \'M\', bitwidth);\n' % (i, j, i, j)
    content += '\n'

    for i in range(1, gadget.nbShares + 1):
        for j in range(i + 1, gadget.nbShares + 1):
            content += '    HWElement & r%d_%d = inputGate(n_r%d_%d);\n' % (i, j, i, j)
            content += '    HWElement & r%d_%d = inputGate(n_r%d_%d);\n' % (j, i, i, j)
    content += '\n'

    for i in range(1, gadget.nbShares + 1):
        for j in range(i + 1, gadget.nbShares + 1):
            content += '    HWElement & r_r%d_%d = Register(r%d_%d);\n' % (i, j, i, j)
            content += '    HWElement & r_r%d_%d = Register(r%d_%d);\n' % (j, i, j, i)
    content += '\n'


    content += '    //---------------HPC2 multiplication-------------------\n'

    for i in range(1, gadget.nbShares + 1):
        for j in range(1, gadget.nbShares + 1):
            if(j!=i):
                content += '    HWElement & u%d_%d = genericGmul(notGate(%s%d), r_r%d_%d);\n' % (i, j, inputVars[0], i, i, j)
    content += '\n'

    for i in range(1, gadget.nbShares + 1):
        for j in range(1, gadget.nbShares + 1):
            if(j!=i):
                content += '    HWElement & v%d_%d = xorGate(%s%d, r%d_%d);\n' % (i, j, inputVars[1], j, i, j)
    content += '\n'

    for i in range(1, gadget.nbShares + 1):
        for j in range(1, gadget.nbShares + 1):
            if(j!=i):
                content += '    HWElement & xor%d_%d = xorGate(Register(u%d_%d), Register(genericGmul(%s%d, Register(v%d_%d))));\n' % (i, j, i, j, inputVars[0], i, i, j)
                if(j==1 or (i==1 and j==2)):
                    content += '    HWElement & sumXor%d_%d = xor%d_%d;\n' % (i, j, i, j)
                elif(j==i+1):
                    content += '    HWElement & sumXor%d_%d = xorGate(sumXor%d_%d, xor%d_%d);\n' % (i, j, i, j-2, i, j)
                else:
                    content += '    HWElement & sumXor%d_%d = xorGate(sumXor%d_%d, xor%d_%d);\n' % (i, j, i, j-1, i, j)
                lastj = j 
        content += '    HWElement & bigXor%d = sumXor%d_%d;\n' % (i, i, lastj)
    content += '\n'

    for i in range(1, gadget.nbShares + 1):
        content += '    HWElement & %s%d = xorGate(Register(genericGmul(%s%d, Register(%s%d))), bigXor%d);\n' % (outputVar, i, inputVars[0], i, inputVars[1], i, i)
    content += '\n'



    exps =  ' ^ '.join(['%s%d.getSymbExp()' % (outputVar, i) for i in range(1, gadget.nbShares + 1)])
    exev =  '%s & %s' % (inputVars[0], inputVars[1])

    content = gadget.verifFunctionality(content, exps, exev)

    content += '    std::vector<HWElement *> outputs;\n'
    content += '\n'
    for i in range(1, gadget.nbShares + 1):
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
    generate_hpc2(*sys.argv[1:])



