/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#ifndef _concrev_hpp_
#define _concrev_hpp_

#include "node.hpp"

bool compareExpsWithExev(Node & e0, Node & e1);
bool compareExpsWithExev(Node & e0, Node & e1, std::map<Node *, Node *> & res, uint64_t * v0, uint64_t * v1);
bool compareExpsWithRandev(Node & e0, Node & e1, int32_t nbEval);
bool compareExpsWithRandev(Node & e0, Node & e1, int32_t nbEval, std::map<Node *, Node *> & res, uint64_t * v0, uint64_t * v1);
bool getDistribWithExev(Node & e, bool * rud);
bool getDistribWithExev(Node & e);



#endif

