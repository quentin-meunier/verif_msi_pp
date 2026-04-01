/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#ifndef _check_leakage_hpp_
#define _check_leakage_hpp_

#include <set>

#include "node.hpp"


bool checkTpsVal(Node & e, bool * usedBitExpRet = NULL);
bool checkNIVal(Node & e, int maxShareOcc, bool * usedBitExpRet = NULL);
bool checkRNIVal(Node & e, int diff, bool * usedBitExpRet = NULL);
bool checkPINIVal(Node & e, int maxShareOcc, std::set<int> & outputIndexes, bool * usedBitExpRet = NULL);
bool checkOPINIVal(Node & e, int maxShareOcc, std::set<int> & outputIndexes, std::vector<std::vector<Node *>> & allOutputLeakages, bool * usedBitExpRet);


bool checkTpsValBit(Node & e);
bool checkNIValBit(Node & e, int maxShareOcc);
bool checkRNIValBit(Node & e, int diff);
bool checkPINIValBit(Node & e, int maxShareOcc, std::set<int> & outputIndexes);
bool checkOPINIValBit(Node & e, int maxShareOcc, std::set<int> & outputIndexes, std::vector<std::vector<Node *>> & allOutputLeakages);


bool checkTpsTrans(Node & e0, Node & e1, bool * usedBitExpRet = NULL);
bool checkNITrans(Node & e0, Node & e1, int maxShareOcc, bool * usedBitExpRet = NULL);
bool checkRNITrans(Node & e0, Node & e1, int diff, bool * usedBitExpRet = NULL);
bool checkPINITrans(Node & e0, Node & e1, int maxShareOcc, std::set<int> & outputIndexes, bool * usedBitExpRet = NULL);
bool checkOPINITrans(Node & e0, Node & e1, int maxShareOcc, std::set<int> & outputIndexes, std::vector<std::vector<Node *>> & allOutputLeakages, bool * usedBitExpRet);


#endif

