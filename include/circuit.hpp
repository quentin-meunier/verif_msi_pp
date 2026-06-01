/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author(s): Quentin L. Meunier, Lucie Chauviere
 */

#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP


#include <cstring>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "verif_msi_pp.hpp"


typedef struct _GadgetParams {
    int32_t order;
    int32_t specifiedOrder;
    SecurityProperty secProp;
    bool withGlitches;
    bool noFalsePositive;
    bool dumpCirc;
    bool checkFunctionality;
    bool multMode;
    int32_t bitwidth;
    const char * circuitFilename;
    const char * outputTarget;
    const char * article;
    const char *  description;
    std::ofstream file;
    std::streambuf * backup;
} GadgetParams;


void defaultParams(GadgetParams & params);

void usage(GadgetParams & params, const char * programmeName);

int32_t verification(GadgetParams & params, Node & exps, Node & exev, std::vector<HWElement *> & outputs, int32_t * nbCheck);

void parseArgs(GadgetParams & params, int cmpt, const char ** tabArgs);

std::vector<Node *> getShares(GadgetParams & params, Node & s, int32_t nbShares);

void displayConfig(GadgetParams & params, const char * benchmark, int32_t nbshares);

HWElement & genericGmul(GadgetParams & params, HWElement & child0, HWElement & child1);


#endif

