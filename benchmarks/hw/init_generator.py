# Copyright (C) 2026, Sorbonne Universite, LIP6
# This file is part of the VerifMSI++ project, under the GPL v3.0 license
# See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
# SPDX-License-Identifier: GPL-3.0-only
# Author(s): Lucie Chauvière, Quentin L. Meunier

import sys
import os



class Gadget:

    def __init__(self, nbShares = 3, order = 2, prop = 'ni', withGlitches = False, noFalsePositive = False, benchmark = None, outfile = None, currentScript = None, bitwidth = 1, multMode = False, withAdditionalRand = False, outputTarget = 'terminal', article = None, descriptionGenerator = None, descriptionFile = None, verbose = False):
        self.nbShares = nbShares
        self.order = order
        self.prop = prop
        self.withGlitches = withGlitches
        self.noFalsePositive = noFalsePositive
        self.benchmark = benchmark
        self.outfile = outfile
        self.currentScript = currentScript
        self.multMode = multMode
        self.bitwidth = bitwidth
        self.withAdditionalRand = withAdditionalRand    # only used for isw_and
        self.outputTarget = outputTarget
        self.article = article
        self.descriptionGenerator = descriptionGenerator
        self.descriptionFile = descriptionFile
        self.verbose = verbose


    def usage(self):
        print('Usage: %s [options]' % self.currentScript)
        print('%s' % self.descriptionGenerator)
        print('Options:')
        print('-f,   --outfile <file>         : Set the name of the generated output file to <file> (default: %s_gen_<n>_shares)' % self.benchmark)
        print('-n,   --nb-shares <n>          : Set the number of shares in the scheme to <n> (default: %d)' % self.nbShares)
        print('-o,   --order <o>              : Set the order of the verification to <o> (default: <n> - 1)')
        print('-bw,  --bitwidth <bw>          : Set the bitwidth of the inputs to <bw> (default: %d)' % self.bitwidth)
        print('-p,   --prop <p>               : Set security property to verify to <p>: either \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference), \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) or \'tps\' (Treshold Probing Security). NI, SNI, RNI, PINI and OPINI use a share description for the inputs, while TPS uses a secrets + masks description (default: %s)' % self.prop)
        print('-m,   --mult-mode              : Use multiplication gadget instead of and (defaut: %s)' % (self.multMode and 'Yes' or 'No'))
        print('-nm,  --without-mult-mode      : Do not use multiplication gadget (defaut: %s)' % (self.multMode and 'No' or 'Yes'))
        print('-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: %s)' % (self.withGlitches and 'Yes' or 'No'))
        print('-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: %s)' % (self.withGlitches and 'No' or 'Yes'))
        print('-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positive (defaut: %s)' % (self.noFalsePositive and 'No' or 'Yes'))
        print('-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: %s)' % (self.noFalsePositive and 'Yes' or 'No'))
        print('-v,   --verbose                : Display more details about the computation (defaut: %s)' % (self.verbose and 'Yes' or 'No'))
        print('-v,   --verbose                : Display less details about the computation (defaut: %s)' % (self.verbose and 'No' or 'Yes'))
        print('-t,   --output-target <f>      : Display the output in the terminal (\'terminal\') or in the file <f> (default: %s)' % self.outputTarget)
        if (self.benchmark == 'isw_and' or self.benchmark == 'isw_and_refresh'):
            print('-r,   --with-rand              : Use an additional random for computing expressions of the form a_i & b_j (default: %s),' % (self.withAdditionalRand and 'Yes' or 'No'))
            print('-nr,  --without-rand           : Do not use additional random for computing expressions of the form a_i & b_j (default: %s)' % (self.withAdditionalRand and 'No' or 'Yes'))
        print('')
        print('%s' % self.article)

    #@staticmethod
    def propPy2cpp(self):
        if self.prop == 'tps':
            return 'TPS'
        elif self.prop == 'ni':
            return 'NI'
        elif self.prop == 'sni':
            return 'SNI'
        elif self.prop == 'rni':
            return 'RNI'
        elif self.prop == 'pini':
            return 'PINI'
        elif self.prop == 'opini':
            return 'OPINI'
        assert(False)


    def parseArgs(self):

        idx = 0
        modifOrder = False
        checkBitwidth = False
        argv = sys.argv[1:]
        while idx < len(argv):
            arg = argv[idx]
            if arg == '-h' or arg == '--help':
                self.usage()
                sys.exit(0)
            elif arg == '-f' or arg == '--outfile':
                idx += 1
                self.outfile = argv[idx]
            elif arg == '-n' or arg == '--nb-shares':
                idx += 1
                self.nbShares = int(argv[idx])
            elif arg == '-o' or arg == '--order':
                idx += 1
                self.order = int(argv[idx])
                modifOrder = True
            elif arg == '-bw' or arg == '--bitwidth':
                idx += 1
                self.bitwidth = int(argv[idx])
                if intargv[idx] != 8:
                    checkBitwidth = True
            elif arg == '-p' or arg == '--prop':
                idx += 1
                self.prop = argv[idx]
            elif arg == '-m' or arg == '--mult-mode':
                self.multMode = True
                self.bitwidth = 8
            elif arg == '-nm' or arg == '--without-mult-mode':
                self.multMode = False
            elif arg == '-g' or arg == '--with-glitches':
                self.withGlitches = True
            elif arg == '-ng' or arg == '--without-glitches':
                withGlitches = False
            elif arg == '-fp' or arg == '--with-false-positive':
                self.noFalsePositive = False
            elif arg == '-nfp' or arg == '--without-false-positive':
                self.noFalsePositive = True
            elif arg == '-v' or arg == '--verbose':
                self.verbose = True
            elif arg == '-nv' or arg == '--no-verbose':
                self.verbose = False
            elif arg == '-t' or arg == '--output-target':
                idx += 1
                self.outputTarget = argv[idx]
            elif arg == '-r' or arg == '--with-rand':
                withAdditionalRandom = True
            elif arg == '-nr' or arg == '--without-rand':
                withAdditionalRandom = False
            else:
                print('*** Error: unrecognized option: %s' % arg, file = sys.stderr)
                self.usage()
                sys.exit(1)
            idx += 1

        if self.multMode and checkBitwidth:
            print('*** Error: the bitwidth should be equal to 8 when the multiplication mode is used')
            sys.exit(1)

        if not modifOrder:
            self.order = self.nbShares - 1

        if self.outfile == None:
            self.outfile = self.benchmark + '_gen_%d_shares.cpp' % self.nbShares
        
        if self.order >= self.nbShares:
            print('*** Error: the order of verification (%d) must be less than the number of shares (%d)' % (self.order, self.nbShares))
            sys.exit(1)
        
        if self.prop != 'ni' and self.prop != 'sni' and self.prop != 'tps' and self.prop != 'rni' and self.prop != 'pini' and self.prop != 'opini':
            print('*** Error: Unknown security property: %s' % self.prop)
            print('    Valid values are: \'ni\' (Non-Interference), \'sni\' (Strong Non-Interference), \'rni\' (Relaxed Non-Interference), \'pini\' (Probe-Isolating Non-Interference), \'opini\' (Output-PINI) and \'tps\' (Treshold Probing Security)')
            sys.exit(1)



    def globalContent(self, content):
        content += '''/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * File generated by %s
 */

#include <cstring>
#include <fstream>

#include "verif_msi_pp.hpp"


''' % (self.currentScript)

        content += 'const char * benchmark = "%s";\n' % self.benchmark
        content += 'int32_t order = %d;\n' % self.order
        content += 'int32_t nbShares = %d;\n' % self.nbShares
        content += 'SecurityProperty secProp = %s;\n' % self.propPy2cpp()
        content += 'int32_t bitwidth = %d;\n' % self.bitwidth
        content += 'bool withGlitches = %s;\n' % (self.withGlitches and 'true' or 'false')
        content += 'bool noFalsePositive = %s;\n' % (self.noFalsePositive and 'true' or 'false')
        content += 'bool dumpCirc = false;\n'
        content += 'bool checkFunctionality = false;\n'
        content += 'bool multMode = %s;\n' % (self.multMode and 'true' or 'false')
        content += 'const char * circuitFilename = \"circuit.dot\";\n'
        content += 'const char * outputTarget = "%s";\n' % self.outputTarget
        content += 'std::ofstream file;\n'
        content += 'std::streambuf * backup = nullptr;\n'
        content += 'bool verbose = %s;\n' % (self.verbose and 'true' or 'false')
        content += '\n'
        
        content += '''void usage(const char * argv) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "%s" << std::endl;
    std::cout << "   This file was generated using the script %s" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-o,   --order <o>              : Set the order of the verification to <o> (default: " << order << ")" << std::endl;
    std::cout << "-bw,  --bitwidth <bw>          : Set the bitwidth of the inputs to <bw> (default: " << bitwidth << ")" << std::endl;
    std::cout << "-p,   --prop <p>               : Set security property to verify to <p>: either \\\'ni\\\' (Non-Interference), \\\'sni\\\' (Strong Non-Interference), \\\'rni\\\' (Relaxed Non-Interference), \\\'pini\\\' (Probe-Isolating Non-Interference), \\\'opini\\\' (Output-PINI) or \\\'tps\\\' (Treshold Probing Security). NI, SNI, RNI, PINI and OPINI use a share description for the inputs, while TPS uses a secrets + masks description (default: '" << secProp2str(secProp) << "')" << std::endl;
    std::cout << "-m,   --mult-mode              : Use multiplication gadget instead of and (defaut: " << (multMode ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-nm,  --without-mult-mode      : Do not use multiplication gadget (defaut: " << (multMode ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-g,   --with-glitches          : Consider glitch propagation throughout gates (defaut: " << (withGlitches ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-ng,  --without-glitches       : Do not consider glitch propagation throughout gates (defaut: " << (withGlitches ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-fp,  --with-false-positive    : Perform symbolic verification only, can lead to false positives (defaut: " << (noFalsePositive ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-nfp, --without-false-positive : Perform symbolic verification, then enumerate if symbolic verification failed (defaut: " << (noFalsePositive ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-v,   --verbose                : Display more details about the computation (defaut: " << (verbose ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-nv,  --no-verbose             : Display less details about the computation (defaut: " << (verbose ? "No" : "Yes") << ")" << std::endl;
    std::cout << "-t,   --output-target <f>      : Display the output in the terminal ('terminal') or in the file <f> (default: " << outputTarget << ")" << std::endl;
    std::cout << "-d,   --dump-circuit           : Dump the circuit in dot format in a file named \\\"" << circuitFilename << "\\\" (default: " << (dumpCircuit ? "Yes" : "No") << ")" << std::endl;
    std::cout << "-c,   --check-functionality    : Check the circuit functionality via exhaustive evaluation (default: " << (checkFunctionality ? "Yes" : "No") << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "%s" << std::endl;
}

  
std::vector<Node *> getShares(Node & s, int32_t nbShares) {
    if (secProp == TPS) {
        return getPseudoShares(s, nbShares);
    }
    else {
        return getRealShares(s, nbShares);
    }
}


void displayConfig() {

    if (strcmp(outputTarget, "terminal") != 0) {
        file.open(outputTarget);
        backup = std::cout.rdbuf(file.rdbuf());
    }

    std::cout << "# Checking Security at order " << order << " for the gadget " << benchmark << " with " << nbShares << " shares, with the options:" << std::endl;
    std::cout << "# " << secProp2str(secProp) << " property," << std::endl;
    std::cout << "# Bitwidth: " << bitwidth << "," << std::endl;
    std::cout << "# Multiplication mode: " << (multMode ? "Yes" : "No") << "," << std::endl;
    std::cout << "# " << (withGlitches ? "With glitches" : "No glitches") << "," << std::endl;
    std::cout << "# No False Positive: " << (noFalsePositive ? "Yes" : "No") << std::endl << std::endl;
}


HWElement & genericGmul(HWElement & child0, HWElement & child1) {
    
    if(multMode) {
        return gmulGadget(child0, child1);
    }
    else {
        return andGate(child0, child1);
    }
}
    ''' % (self.descriptionFile, self.currentScript, self.article)
    
        return content

    
    def verifFunctionality(self, content, exps, exev):
        content += '\n'
        content += '    if (checkFunctionality) {\n'
        content += '        bool res = compareExpsWithExev(' + exps + ', ' + exev + ');\n'
        content += '        if (res) {\n'
        content += '            std::cout << "# Functionality (exhaustive evaluation): [OK]" << std::endl;\n'
        content += '        }\n'
        content += '        else {\n'
        content += '            std::cout << "# Functionality (exhaustive evaluation): [KO]" << std::endl;\n'
        content += '        }\n'
        content += '    }\n'
        content += '\n'
        return content
        



    def check(self, content, inputVars, outputVar):        
        content += '    if (dumpCirc) {\n'
        content += '        dumpCircuit(circuitFilename, outputs);\n'
        content += '    }\n'
        content += '\n'
        
        content += '    int32_t nbLeak = checkSecurity(order, withGlitches, secProp, outputs, noFalsePositive, nbCheck);\n'
        content += '    return nbLeak;\n'
        content += '}\n'
        content += '\n'
        content += '\n'
        
        return content


    def initMain(self, content):
        content += '''int main(int argc, const char ** argv) {

    int32_t idx = 1;
    bool checkBitwidth = false;
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
        else if (strcmp(arg, "-bw") == 0 or strcmp(arg, "--bitwidth") == 0) {
            idx += 1;
            bitwidth = atoi(argv[idx]);
            if (atoi(argv[idx]) != 8) {
                checkBitwidth = true;
            }
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
                std::cerr << "    Valid values are: \\\'ni\\\' (Non-Interference), \\\'sni\\\' (Strong Non-Interference), \\\'rni\\\' (Relaxed Non-Interference), \\\'pini\\\' (Probe-Isolating Non-Interference), \\\'opini\\\' (Output-PINI) and \\\'tps\\\' (Treshold Probing Security)" << std::endl;
                exit(1);
            }
        }
        else if (strcmp(arg, "-m") == 0 or strcmp(arg, "--mult-mode") == 0) {
            multMode = true;
            bitwidth = 8;
        }
        else if (strcmp(arg, "-nm") == 0 or strcmp(arg, "--without-mult-mode") == 0) {
            multMode = false;
        }
        else if (strcmp(arg, "-g") == 0 or strcmp(arg, "--with-glitches") == 0) {
            withGlitches = true;
        }
        else if (strcmp(arg, "-ng") == 0 or strcmp(arg, "--without-glitches") == 0) {
            withGlitches = false;
        }
        else if (strcmp(arg, "-fp") == 0 or strcmp(arg, "--with-false-positive") == 0) {
            noFalsePositive = false;
        }
        else if (strcmp(arg, "-nfp") == 0 or strcmp(arg, "--without-false-positive") == 0) {
            noFalsePositive = true;
        }
        else if (strcmp(arg, "-v") == 0 or strcmp(arg, "--verbose") == 0) {
            verbose = true;
        }
        else if (strcmp(arg, "-nv") == 0 or strcmp(arg, "--verbose") == 0) {
            verbose = false;
        }
        else if (strcmp(arg, "-t") == 0 or strcmp(arg, "--output-target") == 0) {
            idx += 1;
            outputTarget = argv[idx];
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

    if (multMode and checkBitwidth) {
        std::cerr << "*** Error: the bitwidth should be equal to 8 when the multiplication mode is used" << std::endl;
        exit(1);
    }

    if (order >= %d) {
        std::cerr << "*** Error: the order of verification (%d) must be less than the number of shares (%d)" << std::endl;
        exit(1);
    } 


    displayConfig();

    int32_t nbCheck;
    int32_t nbLeak = %s_%d_shares(&nbCheck);
    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    if (backup != nullptr) {
        std::cout.rdbuf(backup);
        file.close();
    }
    
    return 0;
}

''' % (self.nbShares, self.nbShares, self.order, self.benchmark, self.nbShares)

        return content






