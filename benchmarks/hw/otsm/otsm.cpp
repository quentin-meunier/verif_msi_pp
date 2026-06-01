/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author: Quentin L. Meunier, Lucie Chauvière
 */

#include <cstring>

#include "verif_msi_pp.hpp"


bool verbose = false;


int32_t otsm(GadgetParams & params, int32_t * nbCheck) {

    Node & k1 = symbol("k1", 'S', params.bitwidth);
    Node & k2 = symbol("k2", 'S', params.bitwidth);

    std::vector<Node *> a = getShares(params, k1, 2);
    std::vector<Node *> b = getShares(params, k2, 2);

    Node & k10 = *a[0];
    Node & k11 = *a[1];
    Node & k20 = *b[0];
    Node & k21 = *b[1];

    Node & z0 = symbol("z0", 'M', params.bitwidth);
    Node & z1 = symbol("z1", 'M', params.bitwidth);
    Node & z2 = symbol("z2", 'M', params.bitwidth);
    Node & z3 = symbol("z3", 'M', params.bitwidth);
    Node & z4 = symbol("z4", 'M', params.bitwidth);
    Node & z5 = symbol("z5", 'M', params.bitwidth);


    HWElement & x0 = inputGate(k10);
    HWElement & x1 = inputGate(k11);
    HWElement & y0 = inputGate(k20);
    HWElement & y1 = inputGate(k21);
    HWElement & r0 = inputGate(z0);
    HWElement & r1 = inputGate(z1);
    HWElement & r2 = inputGate(z2);
    HWElement & r3 = inputGate(z3);
    HWElement & r4 = inputGate(z4);
    HWElement & r5 = inputGate(z5);

    HWElement & x1y1 = genericGmul(params, x1, y1);
    HWElement & x0y0 = genericGmul(params, x0, y0);
    HWElement & x0r2 = genericGmul(params, x0, r2);
    HWElement & y0r0 = genericGmul(params, y0, r0);
    HWElement & x0r3 = genericGmul(params, x0, r3);
    HWElement & y0r1 = genericGmul(params, y0, r1);
    
    std::vector<HWElement *> vect_x1_p = { &x1, &r0, &r1 };
    HWElement & x1_p = xorGate(vect_x1_p);
    std::vector<HWElement *> vect_y1_p = { &y1, &r2, &r3 };
    HWElement & y1_p = xorGate(vect_y1_p);
    std::vector<HWElement *> vect_xy1_p = { &x1y1, &r4, &r5 };
    HWElement & xy1_p = xorGate(vect_xy1_p);
    std::vector<HWElement *> vect_fc1 = { &x0y0, &x0r2, &y0r0, &r4 };
    HWElement & fc1 = xorGate(vect_fc1);
    std::vector<HWElement *> vect_fc2 = { &x0r3, &y0r1, &r5 };
    HWElement & fc2 = xorGate(vect_fc2);

    HWElement & r_x0 = Register(x0);
    HWElement & r_y1_p = Register(y1_p);
    HWElement & r_y0 = Register(y0);
    HWElement & r_x1_p = Register(x1_p);
    HWElement & r_fc1 = Register(fc1);
    HWElement & r_fc2 = Register(fc2);
    
    
    HWElement & x0y1_p = genericGmul(params, r_x0, r_y1_p);
    HWElement & y0x1_p = genericGmul(params, r_y0, r_x1_p);

    std::vector<HWElement *> vect_f0 = { &x0y1_p, &y0x1_p, &r_fc1, &r_fc2 };

    HWElement & f0 = xorGate(vect_f0);
    HWElement & f1 = Register(xy1_p);


    Node & exps = f0.getSymbExp() ^ f1.getSymbExp();
    Node & exev = k1 & k2;

    std::vector<HWElement *> outputs { &f0, &f1 };


    int32_t nbLeak = verification(params, exps, exev, outputs, nbCheck);

    return nbLeak;
}


int main(int argc, const char ** argv) {

    // Initialisation
    GadgetParams params;
    defaultParams(params);

    params.order = 1; // Shouldn't be changed, designed for order 1 security
    params.secProp = OPINI;
    params.description = "   This script contains a VerifMSI++ description of the OTSM gadget from [1], specified for 2 shares.";
    params.article = "[1] H. Rahimi & A. Moradi (2026). TSM+ and OTSM-Correct Application of Time Sharing Masking in Round-Based Designs. Cryptology ePrint Archive. https://eprint.iacr.org/2026/004";
    
    parseArgs(params, argc, argv);
    displayConfig(params, "otsm", 2);

    int32_t nbCheck;
    int32_t nbLeak = otsm(params, &nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    if (params.backup != nullptr) {
        std::cout.rdbuf(params.backup);
        params.file.close();
    }

    return 0;
}

