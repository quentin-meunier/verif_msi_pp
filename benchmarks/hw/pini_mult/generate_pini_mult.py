# Copyright (C) 2023, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Quentin L. Meunier

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from init_generator import Gadget
 


def generate_pini_mult(*argv):

    gadget = Gadget(prop = 'pini', benchmark = 'pini_mult', currentScript = os.path.basename(__file__), article = '[1] Wang, W., Ji, F., Zhang, J., & Yu, Y. (2023). Efficient Private Circuits with Precomputation. IACR Transactions on Cryptographic Hardware and Embedded Systems.', description_generator = '   This script generates a VerifMSI++ file describing a circuit implementing the finite field multiplication following the PINI scheme from [1].')

    gadget.description_file = "   This script contains a VerifMSI++ description of a circuit implementing the finite field multiplication following the PINI scheme from [1] with %d shares." % (gadget.nbShares)

    gadget.parseArgs()
    
    content = ''
    
    content = gadget.globalContent(content)

    content += '''
   
int32_t pini_mult_%d_shares(int32_t * nbCheck) {

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
        for j in range(1, i):
            content += '    Node & n_r%d_%d = symbol(\"r%d_%d\", \'M\', bitwidth);\n' % (i, j, i, j)
    content += '\n'

    for i in range(1, gadget.nbShares + 1):
        for j in range(1, i):
            content += '    HWElement & r%d_%d = inputGate(n_r%d_%d);\n' % (i, j, i, j)
    content += '\n'

    content += '    HWElement & cst1 = inputGate(constant(1, bitwidth));\n'
    content += '\n'

    def mul_k(k):
        nonlocal content
        if k == 1:
            content += '    HWElement & %s1_1 = genericGmul(%s1, %s1);\n' % (outputVar, inputVars[0], inputVars[1])
            return
        mul_k(k - 1)
        for i in range(1, k):
            content += '    HWElement & u%d_%d = %s%d_%d;\n' % (i, k, outputVar, i, k - 1)

        for i in range(1, k):
            content += '    HWElement & r%d_k%d = xorGate(u%d_%d, r%d_%d);\n' % (i, k, i, k, k, i)
        for i in range(1, k):
            content += '    Node & ruleSrc0%d_%d = ((%s%d.getSymbExp() ^ r%d_k%d.getSymbExp()) & %s%d.getSymbExp()) ^ ((constant(1, bitwidth) ^ %s%d.getSymbExp()) & r%d_k%d.getSymbExp());\n' % (i, k, inputVars[0], i, i, k, inputVars[1], k, inputVars[1], k, i, k)
            content += '    Node & ruleDst0%d_%d = (%s%d.getSymbExp() & %s%d.getSymbExp()) ^ r%d_k%d.getSymbExp();\n' % (i, k, inputVars[0], i, inputVars[1], k, i, k)
            content += '    addSpecificSimpRule(simplify(ruleSrc0%d_%d), simplify(ruleDst0%d_%d));\n' % (i, k, i, k)
            content += '    HWElement & s%d_%d = xorGate(genericGmul(xorGate(%s%d, r%d_k%d), %s%d), genericGmul(xorGate(cst1, %s%d), r%d_k%d));\n' % (i, k, inputVars[0], i, i, k, inputVars[1], k, inputVars[1], k, i, k)
            content += '    Node & ruleSrc1%d_%d = ((%s%d.getSymbExp() ^ s%d_%d.getSymbExp()) & %s%d.getSymbExp()) ^ ((constant(1, bitwidth) ^ %s%d.getSymbExp()) & s%d_%d.getSymbExp());\n' % (i, k, inputVars[1], i, i, k, inputVars[0], k, inputVars[0], k, i, k)
            content += '    Node & ruleDst1%d_%d = (%s%d.getSymbExp() & %s%d.getSymbExp()) ^ s%d_%d.getSymbExp();\n' % (i, k, inputVars[1], i, inputVars[0], k, i, k)
            content += '    addSpecificSimpRule(simplify(ruleSrc1%d_%d), simplify(ruleDst1%d_%d));\n' % (i, k, i, k)
            content += '    HWElement & t%d_%d = xorGate(genericGmul(xorGate(%s%d, s%d_%d), %s%d), genericGmul(xorGate(cst1, %s%d), s%d_%d));\n' % (i, k, inputVars[1], i, i, k, inputVars[0], k, inputVars[0], k, i, k)

        content += '    HWElement & %s%d_1 = genericGmul(%s%d, %s%d);\n' % (outputVar, k, inputVars[0], k, inputVars[1], k)
        for i in range(1, k):
            if i != k - 1:
                content += '    HWElement & %s%d_%d = xorGate(%s%d_%d, t%d_%d);\n' % (outputVar, k, i + 1, outputVar, k, i, i, k)
            else:
                # no index for i at last iteration
                content += '    HWElement & %s%d_%d = xorGate(%s%d_%d, t%d_%d);\n' % (outputVar, k, i + 1, outputVar, k, i, i, k)
        for i in range(1, k):
            content += '    HWElement & %s%d_%d = r%d_%d;\n' % (outputVar, i, k, k, i)


    mul_k(gadget.nbShares)
 
    content += '\n'
    for i in range(1, gadget.nbShares + 1):
        content += '    HWElement & %s%d = %s%d_%d;\n' % (outputVar, i, outputVar, i, gadget.nbShares)
    content += '\n'


    
    exps = ' ^ '.join(['%s%d.getSymbExp()' % (outputVar, i) for i in range(1, gadget.nbShares + 1)])
    exev = '%s & %s' % (inputVars[0], inputVars[1])

    content = gadget.verifFunctionality(content, exps, exev)
    
    
    content += '    std::vector<HWElement *> outputs; // only z shares for pini_mult\n'
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
    generate_pini_mult(*sys.argv[1:])




