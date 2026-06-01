/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author: Quentin L. Meunier, Lucie Chauvière
 */

#include <cstring>

#include "verif_msi_pp.hpp"

bool verbose = false;


int32_t tsm(GadgetParams & params, int32_t * nbCheck) {

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

    HWElement & x0 = inputGate(k10);
    HWElement & x1 = inputGate(k11);
    HWElement & y0 = inputGate(k20);
    HWElement & y1 = inputGate(k21);
    HWElement & r0 = inputGate(z0);
    HWElement & r1 = inputGate(z1);
    HWElement & r2 = inputGate(z2);
    HWElement & r3 = inputGate(z3);
    HWElement & r4 = inputGate(z4);
    
    HWElement & x0_p = xorGate(x0, r3);
    HWElement & x1_p = xorGate(x1, r3);
    HWElement & y0_p = xorGate(y0, r4);
    HWElement & y1_p = xorGate(y1, r4);

    HWElement & r_r0 = Register(r0);
    HWElement & r_r1 = Register(r1);
    HWElement & r_r2 = Register(r2);
    HWElement & r_y1_p = Register(y1_p);
    HWElement & r_x1_p = Register(x1_p);

    HWElement & x0y0_p = genericGmul(params, x0_p, y0_p);
    HWElement & r0x0y0_p = xorGate(x0y0_p, r0);
    HWElement & r_r0x0y0_p = Register(r0x0y0_p);
    HWElement & r_r1x0_p = Register(xorGate(x0_p, r1));
    HWElement & r_r2y0_p = Register(xorGate(y0_p, r2));
    HWElement & r1x0y1_p = genericGmul(params, r_r1x0_p, r_y1_p);
    HWElement & r2y0x1_p = genericGmul(params, r_r2y0_p, r_x1_p);
    std::vector<HWElement *> vect_f0 = { &r_r0x0y0_p, &r1x0y1_p, &r2y0x1_p };
    
    HWElement & r1y1_p = genericGmul(params, r_r1, r_y1_p);
    HWElement & r2x1_p = genericGmul(params, r_r2, r_x1_p);
    HWElement & x1y1_p = genericGmul(params, r_x1_p, r_y1_p);
    std::vector<HWElement *> vect_f1 = { &r_r0, &r1y1_p, &r2x1_p, &x1y1_p };

    HWElement & f0 = xorGate(vect_f0);
    HWElement & f1 = xorGate(vect_f1);


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
    params.secProp = PINI;
    //params.outfile = std::filesystem::path(__FILE__).filename().string();
    params.description = "   This script contains a VerifMSI++ description of the TSM gadget from [1], specified for 2 shares.";
    params.article = "[1] H. Rahimi & A. Moradi (2026). TSM+ and OTSM-Correct Application of Time Sharing Masking in Round-Based Designs. Cryptology ePrint Archive. https://eprint.iacr.org/2026/004";
    
    parseArgs(params, argc, argv);
    displayConfig(params, "tsm", 2);

    int32_t nbCheck;
    int32_t nbLeak = tsm(params, &nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    if (params.backup != nullptr) {
        std::cout.rdbuf(params.backup);
        params.file.close();
    }

    return 0;
}

