/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author: Quentin L. Meunier, Lucie Chauvière
 */

#include <cstring>

#include "verif_msi_pp.hpp"


bool verbose = false;


int32_t tsmp_3_inputs(GadgetParams params, int32_t * nbCheck) {

    Node & k1 = symbol("k1", 'S', params.bitwidth);
    Node & k2 = symbol("k2", 'S', params.bitwidth);
    Node & k3 = symbol("k3", 'S', params.bitwidth);

    std::vector<Node *> a = getShares(params, k1, 2);
    std::vector<Node *> b = getShares(params, k2, 2);
    std::vector<Node *> c = getShares(params, k3, 2);

    Node & k10 = *a[0];
    Node & k11 = *a[1];
    Node & k20 = *b[0];
    Node & k21 = *b[1];
    Node & k30 = *c[0];
    Node & k31 = *c[1];

    Node & m0 = symbol("m0", 'M', params.bitwidth);
    Node & m1 = symbol("m1", 'M', params.bitwidth);
    Node & m2 = symbol("m2", 'M', params.bitwidth);
    Node & m3 = symbol("m3", 'M', params.bitwidth);
    Node & m4 = symbol("m4", 'M', params.bitwidth);
    Node & m5 = symbol("m5", 'M', params.bitwidth);
    Node & m6 = symbol("m6", 'M', params.bitwidth);


    HWElement & x0 = inputGate(k10);
    HWElement & x1 = inputGate(k11);
    HWElement & y0 = inputGate(k20);
    HWElement & y1 = inputGate(k21);
    HWElement & z0 = inputGate(k30);
    HWElement & z1 = inputGate(k31);
    HWElement & r0 = inputGate(m0);
    HWElement & r1 = inputGate(m1);
    HWElement & r2 = inputGate(m2);
    HWElement & r3 = inputGate(m3);
    HWElement & r4 = inputGate(m4);
    HWElement & r5 = inputGate(m5);
    HWElement & r6 = inputGate(m6);

    HWElement & x1y1 = genericGmul(params, x1, y1);
    HWElement & x0y0 = genericGmul(params, x0, y0);
    HWElement & x1z1 = genericGmul(params, x1, z1);
    HWElement & x0z0 = genericGmul(params, x0, z0);
    HWElement & y1z1 = genericGmul(params, y1, z1);
    HWElement & y0z0 = genericGmul(params, y0, z0);
    HWElement & x1y1z1 = genericGmul(params, x1y1, z1);
    HWElement & x0y0z0 = genericGmul(params, x0y0, z0);

    HWElement & x1_p = xorGate(x1, r0);
    HWElement & y1_p = xorGate(y1, r1);
    HWElement & z1_p = xorGate(z1, r2);
    HWElement & x1y1_p = xorGate(x1y1, r3);
    HWElement & x1z1_p = xorGate(x1z1, r4);
    HWElement & y1z1_p = xorGate(y1z1, r5);
    HWElement & x1y1z1_p = xorGate(x1y1z1, r6);

    HWElement & x0r5 = genericGmul(params, x0, r5);
    HWElement & y0r4 = genericGmul(params, y0, r4);
    HWElement & z0r3 = genericGmul(params, z0, r3);
    HWElement & x0y0r2 = genericGmul(params, x0y0, r2);
    HWElement & x0z0r1 = genericGmul(params, x0z0, r1);
    HWElement & y0z0r0 = genericGmul(params, y0z0, r0);
    std::vector<HWElement *> vect_fc = { &x0y0z0, &x0r5, &y0r4, &z0r3, &x0y0r2, &x0z0r1, &y0z0r0, &r6 };
    HWElement & fc = xorGate(vect_fc);

    HWElement & r_x0 = Register(x0);
    HWElement & r_y0 = Register(y0);
    HWElement & r_z0 = Register(z0);
    HWElement & r_x1y1_p = Register(x1y1_p);
    HWElement & r_x1z1_p = Register(x1z1_p);
    HWElement & r_y1z1_p = Register(y1z1_p);
    HWElement & r_x1_p = Register(x1_p);
    HWElement & r_y1_p = Register(y1_p);
    HWElement & r_z1_p = Register(z1_p);
    HWElement & r_fc = Register(fc);

    HWElement & x0_y1z1_p = genericGmul(params, r_x0, r_y1z1_p);
    HWElement & y0_x1z1_p = genericGmul(params, r_y0, r_x1z1_p);
    HWElement & z0_x1y1_p = genericGmul(params, r_z0, r_x1y1_p);
    HWElement & x0y0z1_p = genericGmul(params, genericGmul(params, r_x0, r_y0), r_z1_p);
    HWElement & x0z0y1_p = genericGmul(params, genericGmul(params, r_x0, r_z0), r_y1_p);
    HWElement & y0z0x1_p = genericGmul(params, genericGmul(params, r_y0, r_z0), r_x1_p);
    std::vector<HWElement *> vect_f0 = { &x0_y1z1_p, &y0_x1z1_p, &z0_x1y1_p, &x0y0z1_p, &x0z0y1_p, &y0z0x1_p, &r_fc };

    HWElement & f0 = xorGate(vect_f0);
    HWElement & f1 = Register(x1y1z1_p);

    Node & exps = f0.getSymbExp() ^ f1.getSymbExp();
    Node & exev = k1 & k2 & k3;

    std::vector<HWElement *> outputs { &f0, &f1 };


    int32_t nbLeak = verification(params, exps, exev, outputs, nbCheck);

    return nbLeak;
}


int main(int argc, const char ** argv) {

    // Initialisation
    GadgetParams params;
    defaultParams(params);

    params.order = 1; // Shouldn't be changed, designed for order 1 security
    params.secProp = PINI;
    params.description = "   This script contains a VerifMSI++ description of the TSM+ gadget with 3 inputs from [1], specified for 2 shares.";
    params.article = "[1] H. Rahimi & A. Moradi (2026). TSM+ and OTSM-Correct Application of Time Sharing Masking in Round-Based Designs. Cryptology ePrint Archive. https://eprint.iacr.org/2026/004";
    
    parseArgs(params, argc, argv);
    displayConfig(params, "tsm+", 2);


    int32_t nbCheck;
    int32_t nbLeak = tsmp_3_inputs(params, &nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    if (params.backup != nullptr) {
        std::cout.rdbuf(params.backup);
        params.file.close();
    }

    return 0;
}

