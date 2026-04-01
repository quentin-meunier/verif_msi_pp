/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#ifndef _simp_rules_hpp_
#define _simp_rules_hpp_

#include "node.hpp"



void addGenericSimpRule(Node & s, Node & t);
void addSpecificSimpRule(Node & s, Node & t);

Node & getEquiv(Node & e);
Node & getEquiv(Node & e, bool * found);

#endif

