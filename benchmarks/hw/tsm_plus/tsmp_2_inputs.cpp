/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author: Quentin L. Meunier, Lucie Chauvière
 */

#include <cstring>

#include "verif_msi_pp.hpp"


int32_t order = 1; // Shouldn't be changed, designed for order 1 security
SecurityProperty secProp = PINI;
bool withGlitches = true;
bool noFalsePositive = false;
bool dumpCirc = false;
bool checkFunctionality = false;
const char * circuitFilename = "circuit.dot";
int32_t bitwidth = 1;


void usage(const char * argv) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "   This script contains a VerifMSI++ description of the TSM+ gadget with 2 inputs from [1]." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-o,   --order <n>              : Set the order of the verification to (default: " << order << ")" << std::endl;
    std::cout << "-p,   --prop                   : Set security property to verify: either \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference) \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) or \'tps\' (Treshold Probing Security). NI, SNI, RNI, PINI and OPINI use a share description for the inputs, while TPS uses a secrets + masks description (default: '" << secProp2str(secProp) << "')" << std::endl;
    std::cout << "-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: " << (withGlitches ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: " << (withGlitches ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positives (defaut: " << (noFalsePositive ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: " << (noFalsePositive ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-d,   --dump-circuit           : Dump the circuit in dot format in a file named \"" << circuitFilename << "\" (default: " << (dumpCircuit ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-c,   --check-functionality    : Check the circuit functionality via exhaustive evaluation (default: " << (checkFunctionality ? "Yes" : "No") << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "[1] H. Rahimi & A. Moradi (2026). TSM+ and OTSM-Correct Application of Time Sharing Masking in Round-Based Designs. Cryptology ePrint Archive. https://eprint.iacr.org/2026/004" << std::endl;
}

    
std::vector<Node *> getShares(Node & s, int32_t nbShares) {
    if (secProp == TPS) {
        return getPseudoShares(s, nbShares);
    }
    else {
        return getRealShares(s, nbShares);
    }
}



int32_t tsmp_2_inputs(int32_t * nbCheck) {

    Node & k1 = symbol("k1", 'S', bitwidth);
    Node & k2 = symbol("k2", 'S', bitwidth);

    std::vector<Node *> a = getRealShares(k1, 2);
    std::vector<Node *> b = getRealShares(k2, 2);

    Node & k10 = *a[0];
    Node & k11 = *a[1];
    Node & k20 = *b[0];
    Node & k21 = *b[1];

    Node & z0 = symbol("z0", 'M', bitwidth);
    Node & z1 = symbol("z1", 'M', bitwidth);
    Node & z2 = symbol("z2", 'M', bitwidth);


    HWElement & x0 = inputGate(k10);
    HWElement & x1 = inputGate(k11);
    HWElement & y0 = inputGate(k20);
    HWElement & y1 = inputGate(k21);
    HWElement & r0 = inputGate(z0);
    HWElement & r1 = inputGate(z1);
    HWElement & r2 = inputGate(z2);

    HWElement & x1y1 = andGate(x1, y1);
    HWElement & x0y0 = andGate(x0, y0);
    HWElement & x0r1 = andGate(x0, r1);
    HWElement & y0r0 = andGate(y0, r0);

    HWElement & x1_p = xorGate(x1, r0);
    HWElement & y1_p = xorGate(y1, r1);
    HWElement & x1y1_p = xorGate(x1y1, r2);

    HWElement & y1r1 = xorGate(y1, r1);
    HWElement & x1r0 = xorGate(x1, r0);
    std::vector<HWElement *> vect0 = {&x0y0, &x0r1, &y0r0, &r2};
    HWElement & bigXor_f0 = xorGate(vect0);

    HWElement & r_x0 = Register(x0);
    HWElement & r_y0 = Register(y0);
    HWElement & r_y1r1 = Register(y1r1);
    HWElement & r_x1r0 = Register(x1r0);
    HWElement & r_bigXor_f0 = Register(bigXor_f0);

    HWElement & x0y1r1 = andGate(r_x0, r_y1r1);
    HWElement & y0x1r0 = andGate(r_y0, r_x1r0);
    std::vector<HWElement *> vect_f0 = {&x0y1r1, &y0x1r0, &r_bigXor_f0};
    

    HWElement & f0 = xorGate(vect_f0);
    HWElement & f1 = Register(x1y1_p);

    if (checkFunctionality) {
        bool res = compareExpsWithExev(f0.getSymbExp() ^ f1.getSymbExp(), k1 & k2);
        if (res) {
            std::cout << "# Functionality (exhaustive evaluation): [OK]" << std::endl;
        }
        else {
            std::cout << "# Functionality (exhaustive evaluation): [KO]" << std::endl;
        }
    }

    std::vector<HWElement *> outputs { &f0, &f1 };

    if (dumpCirc) {
        dumpCircuit(circuitFilename, outputs);
    }


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
    int32_t nbLeak = tsmp_2_inputs(&nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    return 0;
}

