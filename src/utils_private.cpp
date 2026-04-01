/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#include "node.hpp"
#include "config.hpp"


void tpsValidity(Node & n) {
    if (n.shareOcc->size() != 0) {
        std::cerr << "*** Error: Threshold Probing verification should not use a share representation but explicit secret variables and masks" << std::endl;
        exit(EXIT_FAILURE);
    }
}


void niValidity(Node & n) {
    if (n.secretVarOcc->size() != 0) {
        std::cerr << "*** Error: NI verification should use a share representation and not explicit secret variables" << std::endl;
        exit(EXIT_FAILURE);
    }
}


void rniValidity(Node & n) {
    if (n.secretVarOcc->size() != 0) {
        std::cerr << "*** Error: RNI verification should use a share representation and not explicit secret variables" << std::endl;
        exit(EXIT_FAILURE);
    }
}


void piniValidity(Node & n) {
    if (n.secretVarOcc->size() != 0) {
        std::cerr << "*** Error: PINI verification should use a share representation and not explicit secret variables" << std::endl;
        exit(EXIT_FAILURE);
    }
}



