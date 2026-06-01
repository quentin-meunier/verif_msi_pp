/* Copyright (C) 2023, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author: Quentin L. Meunier
 */

#include <cstring>

#include "verif_msi_pp.hpp"


bool verbose = false;


int32_t dom_and_5_shares(GadgetParams & params, int32_t * nbCheck) {

    Node & a = symbol("a", 'S', params.bitwidth);
    Node & b = symbol("b", 'S', params.bitwidth);

    std::vector<Node *> v_a = getShares(params, a, 5);
    std::vector<Node *> v_b = getShares(params, b, 5);

    Node & n_a0 = *v_a[0];
    Node & n_a1 = *v_a[1];
    Node & n_a2 = *v_a[2];
    Node & n_a3 = *v_a[3];
    Node & n_a4 = *v_a[4];

    Node & n_b0 = *v_b[0];
    Node & n_b1 = *v_b[1];
    Node & n_b2 = *v_b[2];
    Node & n_b3 = *v_b[3];
    Node & n_b4 = *v_b[4];


    Node & n_z12 = symbol("z12", 'M', params.bitwidth);
    Node & n_z13 = symbol("z13", 'M', params.bitwidth);
    Node & n_z14 = symbol("z14", 'M', params.bitwidth);
    Node & n_z15 = symbol("z15", 'M', params.bitwidth);
    Node & n_z23 = symbol("z23", 'M', params.bitwidth);
    Node & n_z24 = symbol("z24", 'M', params.bitwidth);
    Node & n_z25 = symbol("z25", 'M', params.bitwidth);
    Node & n_z34 = symbol("z34", 'M', params.bitwidth);
    Node & n_z35 = symbol("z35", 'M', params.bitwidth);
    Node & n_z45 = symbol("z45", 'M', params.bitwidth);
    
    HWElement & a0 = inputGate(n_a0);
    HWElement & a1 = inputGate(n_a1);
    HWElement & a2 = inputGate(n_a2);
    HWElement & a3 = inputGate(n_a3);
    HWElement & a4 = inputGate(n_a4);
    HWElement & b0 = inputGate(n_b0);
    HWElement & b1 = inputGate(n_b1);
    HWElement & b2 = inputGate(n_b2);
    HWElement & b3 = inputGate(n_b3);
    HWElement & b4 = inputGate(n_b4);

    HWElement & z12 = inputGate(n_z12);
    HWElement & z13 = inputGate(n_z13);
    HWElement & z14 = inputGate(n_z14);
    HWElement & z15 = inputGate(n_z15);
    HWElement & z23 = inputGate(n_z23);
    HWElement & z24 = inputGate(n_z24);
    HWElement & z25 = inputGate(n_z25);
    HWElement & z34 = inputGate(n_z34);
    HWElement & z35 = inputGate(n_z35);
    HWElement & z45 = inputGate(n_z45);


    // Non linear layer
    HWElement & a0b0 = genericGmul(params, a0, b0);
    HWElement & a0b1 = genericGmul(params, a0, b1);
    HWElement & a0b2 = genericGmul(params, a0, b2);
    HWElement & a0b3 = genericGmul(params, a0, b3);
    HWElement & a0b4 = genericGmul(params, a0, b4);
    HWElement & a1b0 = genericGmul(params, a1, b0);
    HWElement & a1b1 = genericGmul(params, a1, b1);
    HWElement & a1b2 = genericGmul(params, a1, b2);
    HWElement & a1b3 = genericGmul(params, a1, b3);
    HWElement & a1b4 = genericGmul(params, a1, b4);
    HWElement & a2b0 = genericGmul(params, a2, b0);
    HWElement & a2b1 = genericGmul(params, a2, b1);
    HWElement & a2b2 = genericGmul(params, a2, b2);
    HWElement & a2b3 = genericGmul(params, a2, b3);
    HWElement & a2b4 = genericGmul(params, a2, b4);
    HWElement & a3b0 = genericGmul(params, a3, b0);
    HWElement & a3b1 = genericGmul(params, a3, b1);
    HWElement & a3b2 = genericGmul(params, a3, b2);
    HWElement & a3b3 = genericGmul(params, a3, b3);
    HWElement & a3b4 = genericGmul(params, a3, b4);
    HWElement & a4b0 = genericGmul(params, a4, b0);
    HWElement & a4b1 = genericGmul(params, a4, b1);
    HWElement & a4b2 = genericGmul(params, a4, b2);
    HWElement & a4b3 = genericGmul(params, a4, b3);
    HWElement & a4b4 = genericGmul(params, a4, b4);

    // Linear Layer
    HWElement & l0 = xorGate(a1b3, a3b1);
    HWElement & l1i = xorGate(a1b1, a0b1);
    HWElement & l1 = xorGate(l1i, a1b0);
    HWElement & l2 = xorGate(a3b4, a4b3);
    HWElement & l3i = xorGate(a0b4, a4b0);
    HWElement & l3 = xorGate(l3i, a0b0);
    HWElement & l4 = xorGate(a2b4, a4b2);
    HWElement & l5i = xorGate(a4b4, a1b4);
    HWElement & l5 = xorGate(l5i, a4b1);
    HWElement & l6 = xorGate(a2b3, a3b2);
    HWElement & l7i = xorGate(a3b3, a0b3);
    HWElement & l7 = xorGate(l7i, a3b0);
    HWElement & l8 = xorGate(a1b2, a2b1);
    HWElement & l9i = xorGate(a2b2, a0b2);
    HWElement & l9 = xorGate(l9i, a2b0);
    
    
    // Refreshing Layer
    HWElement & c00i = xorGate(l0, z45);
    HWElement & c00 = xorGate(c00i, z12);
    HWElement & c01i = xorGate(l1, z12);
    HWElement & c01 = xorGate(c01i, z13);
    HWElement & c10i = xorGate(l2, z13);
    HWElement & c10 = xorGate(c10i, z14);
    HWElement & c11i = xorGate(l3, z14);
    HWElement & c11 = xorGate(c11i, z15);
    HWElement & c20i = xorGate(l4, z15);
    HWElement & c20 = xorGate(c20i, z23);
    HWElement & c21i = xorGate(l5, z23);
    HWElement & c21 = xorGate(c21i, z24);
    HWElement & c30i = xorGate(l6, z24);
    HWElement & c30 = xorGate(c30i, z25);
    HWElement & c31i = xorGate(l7, z25);
    HWElement & c31 = xorGate(c31i, z34);
    HWElement & c40i = xorGate(l8, z34);
    HWElement & c40 = xorGate(c40i, z35);
    HWElement & c41i = xorGate(l9, z35);
    HWElement & c41 = xorGate(c41i, z45);
    
    
    // Compression Layer
    HWElement & c0 = xorGate(c00, c01);
    HWElement & c1 = xorGate(c10, c11);
    HWElement & c2 = xorGate(c20, c21);
    HWElement & c3 = xorGate(c30, c31);
    HWElement & c4 = xorGate(c40, c41);
 
    
    Node & exps = c0.getSymbExp() ^ c1.getSymbExp() ^ c2.getSymbExp() ^ c3.getSymbExp() ^ c4.getSymbExp();
    Node & exev = a & b;

    std::vector<HWElement *> outputs; // only c shares for gms_and

    outputs.push_back(&c0);
    outputs.push_back(&c1);
    outputs.push_back(&c2);
    outputs.push_back(&c3);
    outputs.push_back(&c4);

    std::vector<std::vector<HWElement *>> outputList;
    outputList.push_back(outputs);

    int32_t nbLeak = verification(params, exps, exev, outputs, nbCheck);
    return nbLeak;
}


int main(int argc, const char ** argv) {

    // Initialisation
    GadgetParams params;
    defaultParams(params);

    params.order = 2; // Shouldn't be changed, designed for order 2 security
    params.secProp = RNI; // defined as "probing" but matches the RNI definition
    params.specifiedOrder = 2;
    params.description = "   This script contains a VerifMSI++ description of a circuit implementing the logical AND following the GMS scheme with 5 shares from [1], designed for order 2 security, using the RNI property with glitches.";
    params.article = "[1] Reparaz, O., Bilgin, B., Nikova, S., Gierlichs, B., & Verbauwhede, I. (2015). Consolidating masking schemes. 35th Annual Cryptology Conference, 2015. Springer Berlin Heidelberg.";
    
    parseArgs(params, argc, argv);
    displayConfig(params, "gms_and", 5);


    int32_t nbCheck;
    int32_t nbLeak = dom_and_5_shares(params, &nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    if (params.backup != nullptr) {
        std::cout.rdbuf(params.backup);
        params.file.close();
    }

    return 0;
}

