/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#ifndef __utils_hpp_
#define __utils_hpp_

#include <vector>
#include <functional>

#include "node.hpp"
#include "arrayexp.hpp"

bool bitExpEnable();

void registerArray(std::string name, int32_t inWidth, int32_t outWidth, uint64_t addr, int32_t size, std::function<Node &(Node *, Node &)> func, void * content, int32_t elemSize);
ArrayExp & getArrayByAddr(uint64_t addr);
ArrayExp & getArrayByName(std::string name);
int32_t getArraySizeByAddr(uint64_t addr);
int32_t getArraySizeByName(std::string name);
std::function<Node &(Node *, Node &)> getArrayFuncByAddr(uint64_t addr);
std::function<Node &(Node *, Node &)> getArrayFuncByName(std::string name);
ArrayExp & getArrayAndOffset(Node & addr, Node ** offset);

void checkResults(Node & res, Node & ref);
void checkResults(Node & res, Node & ref, bool pei, bool usbv);
Node & symbol(const char * symb, char symbType, int32_t width);
Node & constant(int64_t val, int32_t width);
bool litteralInteger(Node & e, uint64_t * val);
void checkTpsResult(Node & exp, bool expected);
void checkNIResult(Node & exp, int maxShares, bool expected);

uint64_t getTime();

std::vector<Node *> getRealShares(Node & s, int nbShares);
std::vector<Node *> getPseudoShares(Node & s, int nbShares);

Node & replaceSharesWithSecretsAndMasks(Node & e);

int32_t width(Node & e);

void verifMSICleanup();
void dumpConfig(std::ostream &os = std::cout);


#endif

