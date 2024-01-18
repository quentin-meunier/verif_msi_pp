/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#ifndef _simplify_hpp_
#define _simplify_hpp_


#include <vector>


#include "node.hpp"


Node & getBitDecomposition(Node & node);

Node & simplify(Node & n);
Node & simplifyAndNotPEI(Node & node);
Node & simplifyCore(Node & node, bool propagateExtractInwards, bool useSingleBitVariables);

bool equivalence(Node & node0, Node & node1);

#endif

