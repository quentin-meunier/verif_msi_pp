/* Copyright (C) 2023, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author: Quentin L. Meunier
 */

#include <cstring>

#include "verif_msi_pp.hpp"


bool verbose = false;


int32_t gms_and_3_shares(GadgetParams & params, int32_t * nbCheck) {

    Node & a = symbol("a", 'S', params.bitwidth);
    Node & b = symbol("b", 'S', params.bitwidth);

    std::vector<Node *> v_a = getShares(params, a, 3);
    std::vector<Node *> v_b = getShares(params, b, 3);

    Node & n_a0 = *v_a[0];
    Node & n_a1 = *v_a[1];
    Node & n_a2 = *v_a[2];

    Node & n_b0 = *v_b[0];
    Node & n_b1 = *v_b[1];
    Node & n_b2 = *v_b[2];


    Node & n_z12 = symbol("z12", 'M', params.bitwidth);
    Node & n_z13 = symbol("z13", 'M', params.bitwidth);
    Node & n_z23 = symbol("z23", 'M', params.bitwidth);

    HWElement & a0 = inputGate(n_a0);
    HWElement & a1 = inputGate(n_a1);
    HWElement & a2 = inputGate(n_a2);
    HWElement & b0 = inputGate(n_b0);
    HWElement & b1 = inputGate(n_b1);
    HWElement & b2 = inputGate(n_b2);

    HWElement & z12 = inputGate(n_z12);
    HWElement & z13 = inputGate(n_z13);
    HWElement & z23 = inputGate(n_z23);
 
    // Non linear layer
    HWElement & a0b0 = genericGmul(params, a0, b0);
    HWElement & a0b1 = genericGmul(params, a0, b1);
    HWElement & a0b2 = genericGmul(params, a0, b2);
    HWElement & a1b0 = genericGmul(params, a1, b0);
    HWElement & a1b1 = genericGmul(params, a1, b1);
    HWElement & a1b2 = genericGmul(params, a1, b2);
    HWElement & a2b0 = genericGmul(params, a2, b0);
    HWElement & a2b1 = genericGmul(params, a2, b1);
    HWElement & a2b2 = genericGmul(params, a2, b2);

    // Linear Layer
    HWElement & l00 = xorGate(a0b0, a2b0);
    HWElement & l0 = xorGate(l00, a0b2);
    HWElement & l10 = xorGate(a1b0, a0b1);
    HWElement & l1 = xorGate(l10, a1b1);
    HWElement & l20 = xorGate(a2b1, a1b2);
    HWElement & l2 = xorGate(l20, a2b2);

    // Refreshing Layer
    HWElement & c00 = xorGate(l0, z12);
    HWElement & c0 = xorGate(c00, z13);
    HWElement & c10 = xorGate(l1, z12);
    HWElement & c1 = xorGate(c10, z23);
    HWElement & c20 = xorGate(l2, z13);
    HWElement & c2 = xorGate(c20, z23);


    Node & exps = c0.getSymbExp() ^ c1.getSymbExp() ^ c2.getSymbExp();
    Node & exev = a & b;

    std::vector<HWElement *> outputs; // only c shares for gms_and

    outputs.push_back(&c0);
    outputs.push_back(&c1);
    outputs.push_back(&c2);

    std::vector<std::vector<HWElement *>> outputList;
    outputList.push_back(outputs);
    
    int32_t nbLeak = verification(params, exps, exev, outputs, nbCheck);
    return nbLeak;
}


int main(int argc, const char ** argv) {

    // Initialisation
    GadgetParams params;
    defaultParams(params);

    params.order = 1; // Shouldn't be changed, designed for order 1 security
    params.secProp = RNI; // defined as "probing" but matches the RNI definition
    params.description = "   This script contains a VerifMSI++ description of a circuit implementing the logical AND following the GMS scheme with 3 shares from [1], designed for order 1 security, using the RNI property with glitches.";
    params.article = "[1] Reparaz, O., Bilgin, B., Nikova, S., Gierlichs, B., & Verbauwhede, I. (2015). Consolidating masking schemes. 35th Annual Cryptology Conference, 2015. Springer Berlin Heidelberg.";
    
    parseArgs(params, argc, argv);
    displayConfig(params, "gms_and", 3);

    int32_t nbCheck;
    int32_t nbLeak = gms_and_3_shares(params, &nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    if (params.backup != nullptr) {
        std::cout.rdbuf(params.backup);
        params.file.close();
    }

    return 0;
}

