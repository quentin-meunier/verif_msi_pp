/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#ifndef _utils_private_hpp_
#define _utils_private_hpp_

#include "node.hpp"
#include "tps.hpp"
#include "hw.hpp"


void secPropValidity(std::set<Node *> & nodes, SecurityProperty secProp);
void secPropValidity(std::set<HWElement *> & gates, SecurityProperty secProp);

void secPropValidity(Node & n, SecurityProperty secProp);

#endif

