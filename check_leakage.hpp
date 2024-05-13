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


bool checkTpsVal(Node & e, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);
bool checkNIVal(Node & e, int maxShareOcc, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);
bool checkRNIVal(Node & e, int diff, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);
bool checkPINIVal(Node & e, int maxShareOcc, std::set<int> & outputIndexes, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);

bool checkTpsValBit(Node & e, uint64_t * timeRet = NULL);
bool checkNIValBit(Node & e, int maxShareOcc, uint64_t * timeRet = NULL);
bool checkRNIValBit(Node & e, int diff, uint64_t * timeRet = NULL);
bool checkPINIValBit(Node & e, int maxShareOcc, std::set<int> & outputIndexes, uint64_t * timeRet = NULL);

bool checkTpsTrans(Node & e0, Node & e1, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);
bool checkNITrans(Node & e0, Node & e1, int maxShareOcc, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);
bool checkRNITrans(Node & e0, Node & e1, int diff, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);
bool checkPINITrans(Node & e0, Node & e1, int maxShareOcc, std::set<int> & outputIndexes, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);


bool checkTpsTransBit(Node & e0, Node & e1, uint64_t * timeRet = NULL);
bool checkTpsTransXor(Node & e0, Node & e1, bool * usedBitExpRet = NULL, uint64_t * timeRet = NULL);
bool checkTpsTransXorBit(Node & e0, Node & e1, uint64_t * timeRet = NULL);


#endif

