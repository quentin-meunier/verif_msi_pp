/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#include "utils_private.hpp"
#include "node.hpp"
#include "hw.hpp"
#include "config.hpp"


static void shareValidity(Node & n, SecurityProperty secProp) {
    if (n.secretVarOcc.size() != 0) {
        std::cerr << "*** Error: " << secProp2str(secProp) << " verification should use a share representation and not explicit secret variables" << std::endl;
        exit(EXIT_FAILURE);
    }
}


static void secretMaskValidity(Node & n, SecurityProperty secProp) {
    if (n.shareOcc.size() != 0) {
        std::cerr << "*** Error: " << secProp2str(secProp) << " verification should not use a share representation but explicit secret variables and masks" << std::endl;
        exit(EXIT_FAILURE);
    }
}



void secPropValidity(std::set<Node *> & nodes, SecurityProperty secProp) {
    for (const auto & n : nodes) {
        secPropValidity(*n, secProp);
    }
}


void secPropValidity(std::set<HWElement *> & gates, SecurityProperty secProp) {
    for (const auto & gate : gates) {
        secPropValidity(*gate->symbExp, secProp);
    }
}


void secPropValidity(Node & n, SecurityProperty secProp) {
    if (secProp == TPS) {
        secretMaskValidity(n, secProp);
    }
    else {
        shareValidity(n, secProp);
    }
}


