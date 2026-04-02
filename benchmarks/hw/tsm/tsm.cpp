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
bool withGlitches = false;
bool noFalsePositive = false;
bool dumpCirc = false;
bool checkFunctionality = true;
const char * circuitFilename = "circuit.dot";
int32_t bitwidth = 1;
std::string article = "[1] H. Rahimi & A. Moradi (2026). TSM+ and OTSM-Correct Application of Time Sharing Masking in Round-Based Designs. Cryptology ePrint Archive. https://eprint.iacr.org/2026/004";


void usage(const char * argv) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "   This script contains a VerifMSI++ description of the TSM gadget from [1]." << std::endl;
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



int32_t tsm(int32_t * nbCheck) {

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
    Node & z3 = symbol("z3", 'M', bitwidth);
    Node & z4 = symbol("z4", 'M', bitwidth);

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

    HWElement & x0y0_p = andGate(x0_p, y0_p);
    HWElement & r0x0y0_p = xorGate(x0y0_p, r0);
    HWElement & r_r0x0y0_p = Register(r0x0y0_p);
    HWElement & r_r1x0_p = Register(xorGate(x0_p, r1));
    HWElement & r_r2y0_p = Register(xorGate(y0_p, r2));
    HWElement & r1x0y1_p = andGate(r_r1x0_p, r_y1_p);
    HWElement & r2y0x1_p = andGate(r_r2y0_p, r_x1_p);
    std::vector<HWElement *> vect_f0 = { &r_r0x0y0_p, &r1x0y1_p, &r2y0x1_p };
    
    HWElement & r1y1_p = andGate(r_r1, r_y1_p);
    HWElement & r2x1_p = andGate(r_r2, r_x1_p);
    HWElement & x1y1_p = andGate(r_x1_p, r_y1_p);
    std::vector<HWElement *> vect_f1 = { &r_r0, &r1y1_p, &r2x1_p, &x1y1_p };

    HWElement & f0 = xorGate(vect_f0);
    HWElement & f1 = xorGate(vect_f1);

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
    int32_t nbLeak = tsm(&nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    return 0;
}

