/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author(s): Quentin L. Meunier, Lucie Chauviere
 */


#include "circuit.hpp"


void defaultParams(GadgetParams & params) {
    params.order = 1;
    params.specifiedOrder = 1;
    params.secProp = PINI;
    params.withGlitches = false;
    params.noFalsePositive = false;
    params.dumpCirc = false;
    params.checkFunctionality = false;
    params.multMode = false;
    params.bitwidth = 1;
    params.circuitFilename = "circuit.dot";
    params.outputTarget = "terminal";
    params.backup = nullptr;
    params.verbose = false;
}



void usage(GadgetParams & params, const char * programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << params.description << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-o,   --order <o>              : Set the order of the verification to <o> (default: " << params.order << ")" << std::endl;
    std::cout << "-bw,  --bitwidth <bw>          : Set the bitwidth of the inputs to <bw> (default: " << params.bitwidth << ")" << std::endl;
    std::cout << "-p,   --prop <p>               : Set security property to verify to <p>: either \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference), \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) or \'tps\' (Treshold Probing Security). NI, SNI, RNI, PINI and OPINI use a share description for the inputs, while TPS uses a secrets + masks description (default: '" << secProp2str(params.secProp) << "')" << std::endl;
    std::cout << "-m,   --mult-mode              : Use multiplication gadget instead of and (defaut: " << (params.multMode ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-nm,  --without-mult-mode      : Do not use multiplication gadget (defaut: " << (params.multMode ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: " << (params.withGlitches ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: " << (params.withGlitches ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positives (defaut: " << (params.noFalsePositive ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: " << (params.noFalsePositive ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-v,   --verbose                : Display more details about the computation (defaut: " << (params.verbose ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-nv,  --no-verbose             : Display less details about the computation (defaut: " << (params.verbose ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-t,   --output-target <f>      : Display the output in the terminal ('terminal') or in the file <f> (default: " << params.outputTarget << ")" << std::endl;
    std::cout << "-d,   --dump-circuit           : Dump the circuit in dot format in a file named \"" << params.circuitFilename << "\" (default: " << (params.dumpCirc ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-c,   --check-functionality    : Check the circuit functionality via exhaustive evaluation (default: " << (params.checkFunctionality ? "Yes" : "No") << ")" << std::endl;
    std::cout << std::endl;
    std::cout << params.article << std::endl;
}



std::vector<Node *> getShares(GadgetParams & params, Node & s, int32_t nbShares) {
    if (params.secProp == TPS) {
        return getPseudoShares(s, nbShares);
    }
    else {
        return getRealShares(s, nbShares);
    }
}


void displayConfig(GadgetParams & params, const char * benchmark, int32_t nbShares){

    if (strcmp(params.outputTarget, "terminal") != 0) {
        params.file.open(params.outputTarget);
        params.backup = std::cout.rdbuf(params.file.rdbuf());
    }

    std::cout << "# Checking Security at order " << params.order << " for the gadget " << benchmark << " with " << nbShares << " shares, with the options:" << std::endl;
    std::cout << "# " << secProp2str(params.secProp) << " property," << std::endl;
    std::cout << "# Bitwidth: " << params.bitwidth << "," << std::endl;
    std::cout << "# Multiplication mode: " << (params.multMode ? "Yes" : "No") << "," << std::endl;
    std::cout << "# " << (params.withGlitches ? "With glitches" : "No glitches") << "," << std::endl;
    std::cout << "# No False Positive: " << (params.noFalsePositive ? "Yes" : "No") << std::endl << std::endl;
}


HWElement & genericGmul(GadgetParams & params, HWElement & child0, HWElement & child1) {
    if (params.multMode) {
        return gmulGadget(child0, child1);
    }
    else {
        return andGate(child0, child1);
    }
}


int32_t verification(GadgetParams & params, Node & exps, Node & exev, std::vector<HWElement *> & outputs, int32_t * nbCheck) {

    if (params.checkFunctionality) {
        bool res = compareExpsWithExev(exps, exev);
        if (res) {
            std::cout << "# Functionality (exhaustive evaluation): [OK]" << std::endl;
        }
        else {
            std::cout << "# Functionality (exhaustive evaluation): [KO]" << std::endl;
        }
    }

    if (params.dumpCirc) {
        dumpCircuit(params.circuitFilename, outputs);
    }


    int32_t nbLeak = checkSecurity(params.order, params.withGlitches, params.secProp, outputs, params.noFalsePositive, nbCheck, params.verbose);

    return nbLeak;
}



void parseArgs(GadgetParams & params, int cmpt, const char ** tabArgs) {

    int32_t idx = 1;
    bool checkBitwidth = false;
    while (idx < cmpt) {
        const char * arg = tabArgs[idx];
        if (strcmp(arg, "-h") == 0 or strcmp(arg, "--help") == 0) {
            usage(params, tabArgs[0]);
            exit(0);
        }
        else if (strcmp(arg, "-o") == 0 or strcmp(arg, "--order") == 0) {
            idx += 1;
            params.order = atoi(tabArgs[idx]);
        }
        else if (strcmp(arg, "-bw") == 0 or strcmp(arg, "--bitwidth") == 0) {
            idx += 1;
            params.bitwidth = atoi(tabArgs[idx]);
            if (atoi(tabArgs[idx]) != 8) {
                checkBitwidth = true;
            }
        }
        else if (strcmp(arg, "-p") == 0 or strcmp(arg, "--prop") == 0) {
            idx += 1;
            const char * prop = tabArgs[idx];
            if (strcmp(prop, "tps") == 0) {
                params.secProp = TPS;
            }
            else if (strcmp(prop, "ni") == 0) {
                params.secProp = NI;
            }
            else if (strcmp(prop, "sni") == 0) {
                params.secProp = SNI;
            }
            else if (strcmp(prop, "rni") == 0) {
                params.secProp =  RNI;
            }
            else if (strcmp(prop, "pini") == 0) {
                params.secProp = PINI;
            }
            else if (strcmp(prop, "opini") == 0) {
                params.secProp = OPINI;
            }
            else {
                std::cerr << "*** Error: Unknown security property: ";
                std::cerr << "    Valid values are: \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference), \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) and \'tps\' (Treshold Probing Security)" << std::endl;
                exit(1);
            }
        }
        else if (strcmp(arg, "-m") == 0 or strcmp(arg, "--mult-mode") == 0) {
            params.multMode = true;
            params.bitwidth = 8;
        }
        else if (strcmp(arg, "-nm") == 0 or strcmp(arg, "--without-mult-mode") == 0) {
            params.multMode = false;
        }
        else if (strcmp(arg, "-g") == 0 or strcmp(arg, "--with-glitches") == 0) {
            params.withGlitches = true;
        }
        else if (strcmp(arg, "-ng") == 0 or strcmp(arg, "--without-glitches") == 0) {
            params.withGlitches = false;
        }
        else if (strcmp(arg, "-fp") == 0 or strcmp(arg, "--with-false-positive") == 0) {
            params.noFalsePositive = false;
        }
        else if (strcmp(arg, "-nfp") == 0 or strcmp(arg, "--without-false-positive") == 0) {
            params.noFalsePositive = true;
        }
        else if (strcmp(arg, "-v") == 0 or strcmp(arg, "--verbose") == 0) {
            params.verbose = true;
        }
        else if (strcmp(arg, "-nv") == 0 or strcmp(arg, "--verbose") == 0) {
            params.verbose = false;
        }
        else if (strcmp(arg, "-t") == 0 or strcmp(arg, "--output-target") == 0) {
            idx += 1;
            params.outputTarget = tabArgs[idx];
        }
        else if (strcmp(arg, "-d") == 0 or strcmp(arg, "--dump-circuit") == 0) {
            params.dumpCirc = true;
        }
        else if (strcmp(arg, "-c") == 0 or strcmp(arg, "--check-functionality") == 0) {
            params.checkFunctionality = true;
        }
        else {
            std::cerr << "*** Error: unrecognized option: " << arg << std::endl;
            usage(params, tabArgs[0]);
            exit(1);
        }
        idx += 1;
    }

    if (params.multMode and checkBitwidth) {
        std::cerr << "*** Error: the bitwidth should be equal to 8 when the multiplication mode is used" << std::endl;
        exit(1);
    }
    
    if (params.order > params.specifiedOrder) {
        std::cerr << "*** Error: the order of verification should be less than " << params.specifiedOrder + 1 << " for this gadget" << std::endl;
        exit(1);
    }
    else if (params.order < params.specifiedOrder) {
        std::cout << "*** Warning: this gadget was set for the order " << params.specifiedOrder << std::endl;
    }
}


