/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#ifndef _tps_hpp_
#define _tps_hpp_


#include <set>
#include <string>

#include "node.hpp"


typedef enum _SecurityProperty {
    PROP_NONE,
    TPS,
    NI,
    SNI,
    RNI,
    PINI
} SecurityProperty;


typedef struct _PropParams {
    int maxShareOcc;
    int diff;
    std::set<int> * outputIndexes;
} PropParams;


std::string secProp2str(SecurityProperty secProp);

bool tps(Node & nodeIn, bool bitDecompose = false, bool verbose = false);
bool tps(std::vector<Node *> & nodes, bool bitDecompose = false, bool verbose = false);
bool ni(Node & nodeIn, int maxShareOcc, bool bitDecompose = false, bool verbose = false);
bool ni(std::vector<Node *> & nodes, int maxShareOcc, bool bitDecompose = false, bool verbose = false);
bool rni(Node & nodeIn, int diff, bool bitDecompose = false, bool verbose = false);
bool rni(std::vector<Node *> & nodes, int diff, bool bitDecompose = false, bool verbose = false);
bool pini(Node & nodeIn, int maxShareOcc, std::set<int> & outputIndexes, bool bitDecompose = false, bool verbose = false);
bool pini(std::vector<Node *> & nodes, int maxShareOcc, std::set<int> & outputIndexes, bool bitDecompose = false, bool verbose = false);


#endif

