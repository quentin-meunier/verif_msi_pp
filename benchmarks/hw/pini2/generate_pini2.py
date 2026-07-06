# Copyright (C) 2026, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Lucie Chauvière, Quentin L. Meunier

import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from init_generator import Gadget
 


def generate_pini2(*argv):
    
    gadget = Gadget(prop = 'pini', benchmark = 'pini2', currentScript = os.path.basename(__file__), article = '[1] Gaëtan Cassiers and François-Xavier Standaert. . Towards globally optimized masking: From low randomness to low noise rate or probe isolating multiplications with reduced randomness and security against horizontal attacks. IACR Trans. Cryptogr. Hardw. Embed. Syst., 2019.', descriptionGenerator = '   This script generates a VerifMSI++ file of the PINI2 gadget from [1].')

    gadget.parseArgs()

    gadget.descriptionFile = "   This script contains a VerifMSI++ description of the PINI2 gadget from [1] with %d shares." % (gadget.nbShares)
    
    content = ''
    
    content = gadget.globalContent(content)


    content += '''


int32_t pini2_%d_shares(int32_t * nbCheck) {

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
            content += '    HWElement & α%d%d = genericGmul(%s%d, %s%d);\n' % (i, j, inputVars[0], i, inputVars[1], j)

    #-----------------------init_cprod--------------------------------
    for i in range(gadget.nbShares):
        content += '    Node & ns%d = symbol(\"s%d\", \'M\', bitwidth);\n' % (i, i)
        content += '    HWElement & s%d = inputGate(ns%d);\n' % (i, i)
    content += '\n'

    for i in range(gadget.nbShares):
        for j in range(i + 1, gadget.nbShares):
            content += '    HWElement & s%d%d = xorGate(s%d, s%d);\n' % (i, j, i, j)
            content += '    HWElement & p0_%d%d = genericGmul(%s%d, s%d%d);\n' % (i, j, inputVars[0], i, i, j)
            content += '    HWElement & %s%ds%d%d = xorGate(%s%d, s%d%d);\n' % (inputVars[1], j, i, j, inputVars[1], j, i, j)
            content += '    HWElement & p1_%d%d = genericGmul(%s%d, %s%ds%d%d);\n' % (i, j, inputVars[0], i, inputVars[1], j, i, j)
            content += '    HWElement & p2_%d%d = genericGmul(%s%d, s%d%d);\n' % (i, j, inputVars[1], i, i, j)
            content += '    HWElement & %s%ds%d%d = xorGate(%s%d, s%d%d);\n' % (inputVars[0], j, i, j,inputVars[0], j, i, j)
            content += '    HWElement & p3_%d%d = genericGmul(%s%d, %s%ds%d%d);\n' % (i, j, inputVars[1], i, inputVars[0], j, i, j)
    content += '\n'
    #----------------------------------------------------------------


    for i in range(gadget.order + 1):
        for j in range(0, gadget.order - i, 2):
            content += '    Node & nr%d%d = symbol(\"r%d%d\", \'M\', bitwidth);\n' % (i, gadget.order - j, i, gadget.order - j)
            content += '    HWElement & r%d%d = inputGate(nr%d%d);\n' % (i, gadget.order - j, i, gadget.order - j)
    content += '\n'


    for j in range(gadget.order - 1, 0, -2):
        content += '    Node & nr%d = symbol(\"r%d\", \'M\', bitwidth);\n' % (j, j)
        content += '    HWElement & r%d = inputGate(nr%d);\n' % (j, j)
    content += '\n'


    for i in range(gadget.order + 1):

        if (i + 1) < gadget.order:
            content += '    std::vector<HWElement *> vectSumt%d;\n' % (i)

        for j in range(gadget.order, i + 1, -2):

            #--------------------------cprod-------------------------------
            content += '    std::vector<HWElement *> vectCProd%d%d = {&r%d%d, &p0_%d%d, &p1_%d%d, &p2_%d%d, &p3_%d%d, &r%d, &p0_%d%d, &p1_%d%d, &p2_%d%d, &p3_%d%d};\n' % (i, j, i, j, i, j, i, j, i, j, i, j, j - 1, i, j - 1, i, j - 1, i, j - 1, i, j - 1)
            content += '    HWElement & t%d%d = xorGate(vectCProd%d%d);\n' % (i, j, i, j)
            #--------------------------------------------------------------
            content += '    vectSumt%d.push_back(&t%d%d);\n' %(i, i, j)

        if (i + 1) < gadget.order:
            content += '    HWElement & sumt%d = xorGate(vectSumt%d);\n' % (i, i)
            content += '    HWElement & e%d = xorGate(α%d%d, sumt%d);\n' % (i, i, i, i)
        else:
            content += '    HWElement & e%d = α%d%d;\n' % (i, i, i)
        content += '\n'

        if (i % 2) != (gadget.order % 2):

            #--------------------------cprod'------------------------------
            content += '    std::vector<HWElement *> vectCProdPrim%d = {&r%d%d, &p0_%d%d, &p1_%d%d, &p2_%d%d, &p3_%d%d};\n' % (i, i, i  + 1, i, i  + 1, i, i  + 1, i, i  + 1, i, i  + 1)
            content += '    HWElement & t%d%d = xorGate(vectCProdPrim%d);\n' % (i, i + 1, i)
            #--------------------------------------------------------------

            content += '    HWElement & prec%d = xorGate(e%d, t%d%d);\n' % (i, i, i, i + 1)
            if (i % 2) == 1:
                content += '    HWElement & c%d = xorGate(prec%d, r%d);\n' % (i, i, i)
            else:
                content += '    HWElement & c%d = prec%d;\n' % (i, i)

            content += '\n'

        else:
            
            if (i - 1) >= 0:
                content += '    std::vector<HWElement *> vectSumr%d;\n' % (i)

            for j in range(i - 1, -1, -1):
                content += '    vectSumr%d.push_back(&r%d%d);\n' %(i, j, i)


            if (i - 1) >= 0:
                content += '    HWElement & sumr%d = xorGate(vectSumr%d);\n' % (i, i)
                content += '    HWElement & c%d = xorGate(e%d, sumr%d);\n' % (i, i, i)
            else:
                content += '    HWElement & c%d = e%d;\n' % (i, i)

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
    generate_pini2(*sys.argv[1:])



