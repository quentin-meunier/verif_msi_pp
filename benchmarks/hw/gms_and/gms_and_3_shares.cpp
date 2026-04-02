/* Copyright (C) 2023, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author: Quentin L. Meunier
 */

#include <cstring>

#include "verif_msi_pp.hpp"

int32_t order = 1; // Shouldn't be changed, designed for order 1 security
SecurityProperty secProp = TPS;
bool withGlitches = false;
bool noFalsePositive = false;
bool dumpCirc = true;
bool checkFunctionality = true;
const char * circuitFilename = "circuit.dot";
std::string article = "[1] Reparaz, O., Bilgin, B., Nikova, S., Gierlichs, B., & Verbauwhede, I. (2015). Consolidating masking schemes. 35th Annual Cryptology Conference, 2015. Springer Berlin Heidelberg.";


void usage(const char * argv) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "   This script contains a VerifMSI++ description of a circuit implementing the logical AND following the GMS scheme with 3 shares from [1]." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-o,   --order <n>              : Set the order of the verification to (default: " << order << ")" << std::endl;
    std::cout << "-p,   --prop                   : Set security property to verify: either \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference) \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) or \'tps\' (Treshold Probing Security). NI, SNI, RNI and PINI use a share description for the inputs, while TPS uses a secrets + masks description (default: '" << secProp2str(secProp) << "')" << std::endl;
    std::cout << "-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: " << (withGlitches ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: " << (withGlitches ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positives (defaut: " << (noFalsePositive ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: " << (noFalsePositive ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-d,   --dump-circuit           : Dump the circuit in dot format in a file named \"" << circuitFilename << "\" (default: " << (dumpCircuit ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-c,   --check-functionality    : Check the circuit functionality via exhaustive evaluation (default: " << (checkFunctionality ? "Yes" : "No") << ")" << std::endl;
    std::cout << std::endl;
    std::cout << article << std::endl;
}

    
std::vector<Node *> getShares(Node & s, int32_t nbShares) {
    if (secProp == TPS) {
        return getPseudoShares(s, nbShares);
    }
    else {
        return getRealShares(s, nbShares);
    }
}


int32_t gms_and_3_shares(int32_t * nbCheck) {

    Node & a = symbol("a", 'S', 1);
    Node & b = symbol("b", 'S', 1);

    std::vector<Node *> v_a = getShares(a, 3);
    std::vector<Node *> v_b = getShares(b, 3);

    Node & n_a0 = *v_a[0];
    Node & n_a1 = *v_a[1];
    Node & n_a2 = *v_a[2];

    Node & n_b0 = *v_b[0];
    Node & n_b1 = *v_b[1];
    Node & n_b2 = *v_b[2];


    Node & n_z12 = symbol("z12", 'M', 1);
    Node & n_z13 = symbol("z13", 'M', 1);
    Node & n_z23 = symbol("z23", 'M', 1);

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
    HWElement & a0b0 = andGate(a0, b0);
    HWElement & a0b1 = andGate(a0, b1);
    HWElement & a0b2 = andGate(a0, b2);
    HWElement & a1b0 = andGate(a1, b0);
    HWElement & a1b1 = andGate(a1, b1);
    HWElement & a1b2 = andGate(a1, b2);
    HWElement & a2b0 = andGate(a2, b0);
    HWElement & a2b1 = andGate(a2, b1);
    HWElement & a2b2 = andGate(a2, b2);

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
 

    if (checkFunctionality) {
        bool res = compareExpsWithExev(c0.getSymbExp() ^ c1.getSymbExp() ^ c2.getSymbExp(), a & b);
        if (res) {
            std::cout << "# Functionality (exhaustive evaluation): [OK]" << std::endl;
        }
        else {
            std::cout << "# Functionality (exhaustive evaluation): [KO]" << std::endl;
        }
    }

    std::vector<HWElement *> outputs; // only c shares for gms_and

    outputs.push_back(&c0);
    outputs.push_back(&c1);
    outputs.push_back(&c2);

    std::vector<std::vector<HWElement *>> outputList;
    outputList.push_back(outputs);
    if (dumpCirc) {
        dumpCircuit(circuitFilename, outputs);
    }
    //int32_t nbLeak = checkSecurity(order, withGlitches, secProp, c0, c1, c2)
    int32_t nbLeak = checkSecurity(order, withGlitches, secProp, outputs, noFalsePositive, nbCheck);
    return nbLeak;
}


int main(int argc, const char ** argv) {

    int32_t idx = 1;
    while (idx < argc) {
        const char * arg = argv[idx];
        if (strcmp(arg, "-h") == 0 or strcmp(arg, "--help") == 0) {
            usage(argv[0]);
            exit(0);
        }
        else if (strcmp(arg, "-o") == 0 or strcmp(arg, "--order") == 0) {
            idx += 1;
            order = atoi(argv[idx]);
        }
        else if (strcmp(arg, "-p") == 0 or strcmp(arg, "--prop") == 0) {
            idx += 1;
            const char * prop = argv[idx];
            if (strcmp(prop, "tps") == 0) {
                secProp = TPS;
            }
            else if (strcmp(prop, "ni") == 0) {
                secProp = NI;
            }
            else if (strcmp(prop, "sni") == 0) {
                secProp = SNI;
            }
            else if (strcmp(prop, "rni") == 0) {
                secProp =  RNI;
            }
            else if (strcmp(prop, "pini") == 0) {
                secProp = PINI;
            }
            else if (strcmp(prop, "opini") == 0) {
                secProp = OPINI;
            }
            else {
                std::cerr << "*** Error: Unknown security property: ";
                std::cerr << "    Valid values are: \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference), \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) and \'tps\' (Treshold Probing Security)" << std::endl;
                exit(1);
            }
        }
        else if (strcmp(arg, "-g") == 0 or strcmp(arg, "--with-glitches") == 0) {
            withGlitches = true;
        }
        else if (strcmp(arg, "-ng") == 0 or strcmp(arg, "--without-glitches") == 0) {
            withGlitches = false;
        }
        else if (strcmp(arg, "-d") == 0 or strcmp(arg, "--dump-circuit") == 0) {
            dumpCirc = true;
        }
        else if (strcmp(arg, "-c") == 0 or strcmp(arg, "--check-functionality") == 0) {
            checkFunctionality = true;
        }
        else {
            std::cerr << "*** Error: unrecognized option: " << arg << std::endl;
            usage(argv[0]);
            exit(1);
        }
        idx += 1;
    }
    
    
    if (order >= 2) {
        std::cerr << "*** Error: the order of verification should be 1 for this gadget" << std::endl;
        exit(1);
    } 


    int32_t nbCheck;
    int32_t nbLeak = gms_and_3_shares(&nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    return 0;
}

