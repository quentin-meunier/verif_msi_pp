# Copyright (C) 2023, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Quentin L. Meunier

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from init_generator import Gadget






def generate_dom_and(*argv):
    
    gadget = Gadget(prop = 'ni', benchmark = 'dom_and', currentScript = os.path.basename(__file__), article = '[1] Groß, H., Mangard, S., & Korak, T. (2017). An efficient side-channel protected AES implementation with arbitrary protection order. In Topics in Cryptology–CT-RSA 2017: The Cryptographers\' Track at the RSA Conference 2017, Springer International Publishing.', descriptionGenerator = '   This script generates a VerifMSI++ file describing a circuit implementing the logical AND following the DOM scheme from [1].')

    gadget.parseArgs()

    gadget.descriptionFile = "   This script contains a VerifMSI++ description of a circuit implementing the logical AND following the DOM scheme from [1] with %d shares." % (gadget.nbShares)

    
    content = ''
    
    content = gadget.globalContent(content)

    content += '''

int32_t dom_and_%d_shares(int32_t * nbCheck) {

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
    
    
    
    for i in range(gadget.nbShares):
        for j in range(gadget.nbShares):
            content += '    HWElement & %s%d%s%di = genericGmul(%s%d, %s%d);\n' % (inputVars[0], i, inputVars[1], j, inputVars[0], i, inputVars[1], j)
    
    content += '\n'
    
    
    z_idx = 0
    for i in range(gadget.nbShares):
        for j in range(i):
            content += '    Node & n_z%d_%d = symbol(\"z%d_%d\", \'M\', bitwidth); // Z%d\n' % (i, j, i, j, z_idx)
            content += '    HWElement & z%d_%d = inputGate(n_z%d_%d);\n' % (i, j, i, j)
    
            content += '    HWElement & %s%d%s%dx = xorGate(%s%d%s%di, z%d_%d);\n' % (inputVars[0], i, inputVars[1], j, inputVars[0], i, inputVars[1], j, i, j)
            content += '    HWElement & %s%d%s%dr = Register(%s%d%s%dx);\n' % (inputVars[0], i, inputVars[1], j, inputVars[0], i, inputVars[1], j)
    
            content += '    HWElement & %s%d%s%dx = xorGate(%s%d%s%di, z%d_%d);\n' % (inputVars[0], j, inputVars[1], i, inputVars[0], j, inputVars[1], i, i, j)
            content += '    HWElement & %s%d%s%dr = Register(%s%d%s%dx);\n' % (inputVars[0], j, inputVars[1], i, inputVars[0], j, inputVars[1], i)
    
            z_idx += 1
    content += '\n'
    
    
    for i in range(gadget.nbShares):
        content += '    HWElement & %s%d_0 = %s%d%s%di;\n' % (outputVar, i, inputVars[0], i, inputVars[1], i)
        c_idx = 1
        for j in range(gadget.nbShares):
            if i != gadget.nbShares - 1 and j == gadget.nbShares - 1 or i == gadget.nbShares - 1 and j == gadget.nbShares - 2:
                content += '    HWElement & %s%d = xorGate(%s%d_%d, %s%d%s%dr);\n' % (outputVar, i, outputVar, i, (c_idx - 1), inputVars[0], i, inputVars[1], j)
            elif i != j:
                content += '    HWElement & %s%d_%d = xorGate(%s%d_%d, %s%d%s%dr);\n' % (outputVar, i, c_idx, outputVar, i, (c_idx - 1), inputVars[0], i, inputVars[1], j)
                c_idx += 1


    exps =  ' ^ '.join(['%s%d.getSymbExp()' % (outputVar, i) for i in range(gadget.nbShares)])
    exev =  '%s & %s' % (inputVars[0], inputVars[1])
    
    content = gadget.verifFunctionality(content, exps, exev)
    
    
    content += '    std::vector<HWElement *> outputs; // only c shares for dom_and\n'
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
    generate_dom_and(*sys.argv[1:])




