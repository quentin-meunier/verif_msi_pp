/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#include <cassert>
#include <sys/time.h>

#include "utils.hpp"
#include "node.hpp"
#include "arrayexp.hpp"
#include "simplify.hpp"
#include "check_leakage.hpp"
#include "config.hpp"
#include "hw.hpp"


std::set<Node *> secretShared;
std::set<ArrayExp *> registeredArrays;
std::map<std::string, ArrayExp *> registeredArraysByName;
std::map<uint64_t, ArrayExp *> registeredArraysByAddr;



bool bitExpEnable() {
    return BIT_EXP_ENABLE;
}



void registerArray(std::string name, int32_t inWidth, int32_t outWidth, uint64_t addr, int32_t size, std::function<Node &(Node *, Node &)> func, void * content, int32_t elemSize) {

    if (addr != 0) {
        if (registeredArraysByAddr.contains(addr)) {
            std::cerr << "*** Error: Array with base address 0x" << std::hex << addr << std::dec << " already registered" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    if (registeredArraysByName.contains(name)) {
        std::cerr << "*** Error: Array with name " << name << " already registered" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (inWidth != 8 && inWidth != 16 && inWidth != 32 && inWidth != 64) {
        std::cerr << "*** Error: Only supported values for inWidth are 8, 16, 32 and 64" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (outWidth != 8 && outWidth != 16 && outWidth != 32 && inWidth != 64) {
        std::cerr << "*** Error: Only supported values for outWidth are 8, 16, 32 and 64" << std::endl;
        exit(EXIT_FAILURE);
    }

    ArrayExp * arr = new ArrayExp(name, inWidth, outWidth, addr, size, func, content, elemSize);

    if (addr != 0x0) {
        registeredArraysByAddr.emplace(addr, arr);
    }
    registeredArraysByName.emplace(name, arr);
    registeredArrays.insert(arr);
}



ArrayExp & getArrayByAddr(uint64_t addr) {
    return *registeredArraysByAddr[addr];
}


ArrayExp & getArrayByName(std::string name) {
    return *registeredArraysByName[name];
}


int32_t getArraySizeByAddr(uint64_t addr) {
    return registeredArraysByAddr[addr]->size;
}


int32_t getArraySizeByName(std::string name) {
    return registeredArraysByName[name]->size;
}


std::function<Node &(Node *, Node &)> getArrayFuncByAddr(uint64_t addr) {
    return registeredArraysByAddr[addr]->func;
}

std::function<Node &(Node *, Node &)> getArrayFuncByName(std::string name) {
    return registeredArraysByName[name]->func;
}



ArrayExp & getArrayAndOffset(Node & addr, Node ** offset) {
    ArrayExp * arr = NULL;
    std::vector<Node *> newChildren;

    if (addr.nature == OP && addr.op == PLUS) {
        for (const auto & child : *addr.children) {
            if (child->nature == CONST) {
                assert(child->nlimbs == 1);
                arr = &getArrayByAddr(child->cst[0]);
            }
            else {
                newChildren.push_back(child);
            }
        }
    }
    if (arr != NULL) {
        Node * offNode;
        if (newChildren.size() == 1) {
            offNode = newChildren[0];
        }
        else {
            offNode = &Node::OpNode(PLUS, newChildren);
        }
        if (arr->elemSize != 1) {
            int32_t shiftVal;
            switch (arr->elemSize) {
                case 1:
                    shiftVal = 0;
                    break;
                case 2:
                    shiftVal = 1;
                    break;
                case 4:
                    shiftVal = 2;
                    break;
                case 8:
                    shiftVal = 3;
                    break;
                default:
                    assert(false);
            }
            offNode = &simplify(LShR(*offNode, shiftVal));
        }

        if (offNode->width < arr->inWidth) {
            offNode = &ZeroExt(arr->inWidth - offNode->width, *offNode);
        }
        else if (offNode->width > arr->inWidth) {
            // FIXME: verify that all removed bits are 0?
            offNode = &Extract(arr->inWidth - 1, 0, *offNode);
        }
        *offset = offNode;
        return *arr;
    }

    // FIXME: in case symbolic array of 32-bit integers is at adress 0x1000,
    // and offset is (k ^ m) + 4, array will be searched at address 0x1004 and it will fail
    // This case can be implemented if encountered, but it is very unlikely
    std::cerr << "*** Error: symbolic address for symbolic array access does not contain array base address (address is " << std::hex << addr << std::dec << ")" << std::endl;
    exit(EXIT_FAILURE);
}
        



void checkResults(Node & res, Node & ref) {
    return checkResults(res, ref, true, false);
}


void checkResults(Node & res, Node & ref, bool pei, bool usbv) {
    int32_t nbBits = ref.width;
    
    if (nbBits != res.width) {
        std::cout << "KO (nbBits on res: " << res.width << " -- expected " << nbBits << ")" << std::endl;
    }

    Node & res_s = simplify(res, pei, usbv);
    Node & ref_s = simplify(ref, pei, usbv);

    std::cout << "res : " << res_s << " [" << std::dec << res_s.width << "]" << std::endl;
    std::cout << "ref : " << ref_s << " [" << std::dec << ref_s.width << "]" << std::endl;

    if (nbBits != res.width || nbBits != ref.width) {
        std::cout << "KO (nbBits after simplify: res: " << res.width << " - ref: " << ref.width << " - expected: " << nbBits << ")" << std::endl;
    }
    
    if (equivalence(res_s, ref_s)) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }
}


Node & symbol(const char * symb, char symbType, int32_t width) {
    return Symb(symb, symbType, width, 0, 0, NULL, NULL);
}


Node & constant(int64_t val, int32_t width) {
    return Const((uint64_t) val, width);
}


bool litteralInteger(Node & e, uint64_t * val) {
    if (e.nature == CONST && e.nlimbs == 1) {
        *val = e.cst[0];
        return true;
    }
    else {
        Node & s = simplify(e);
        if (s.nature == CONST && e.nlimbs == 1) {
            *val = s.cst[0];
            return true;
        }
        else {
            return false;
        }
    }
}


void checkTpsResult(Node & exp, bool expected) {
    bool res = checkTpsVal(exp);
    if (res == expected) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }
}


void checkNIResult(Node & exp, int maxShares, bool expected) {
    bool res = checkNIVal(exp, maxShares);
    if (res == expected) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }
}



uint64_t getTime() {
    struct timeval fullTime;
    gettimeofday(&fullTime, NULL);
	return (fullTime.tv_usec + (uint64_t) fullTime.tv_sec * 1000000) / 1000;
}


static std::vector<Node *> getPseudoSharesInternal(Node & s, int nbShares) {
    std::vector<Node *> res;
    for (int32_t i = 0; i < nbShares - 1; i += 1) {
        std::string str(*s.symb);
        std::string ht_char("#");
        if (str.find('#') != std::string::npos) {
            str.replace(str.find(ht_char), ht_char.length(), "@" + std::to_string(i) + ht_char);
        }
        else {
            str = str + "@" + std::to_string(i);
        }

        Node & a = SymbInternal(str, 'M', s.width);
        res.push_back(&a);
    }
    res.push_back(&s);
    Node & a0 = Node::OpNode(BXOR, res);
    res.pop_back();
    res.insert(res.begin(), &a0);
    return res;
}


std::vector<Node *> getRealShares(Node & s, int nbShares) {
    if (s.nature != SYMB || s.symbType != 'S') {
        std::cerr << "*** Error: first parameter of function getRealShares() can only be a secret variable" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (secretShared.contains(&s)) {
        std::cerr << "*** Error: Secret variable " << s << " has already been split into shares" << std::endl;
        exit(EXIT_FAILURE);
    }
    secretShared.insert(&s);
    std::vector<Node *> pseudoShares = getPseudoSharesInternal(s, nbShares);
    std::vector<Node *> res;
    for (int32_t i = 0; i < nbShares; i += 1) {
        std::string str(*s.symb);
        std::string ht_char("#");
        if (str.find('#') != std::string::npos) {
            str.replace(str.find(ht_char), ht_char.length(), "[" + std::to_string(i) + "]" + ht_char);
        }
        else {
            str = str + "[" + std::to_string(i) + "]";
        }

        Node & a = SymbInternal(str, 'A', s.width, nbShares, i, &s, pseudoShares[i]);
        res.push_back(&a);
    }
    return res;
}


std::vector<Node *> getPseudoShares(Node & s, int nbShares) {
    if (s.nature != SYMB || s.symbType != 'S') {
        std::cerr << "*** Error: first parameter of function getPseudoShares() can only be a secret variable" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (secretShared.contains(&s)) {
        std::cerr << "*** Error: Secret variable " << s << " has already been split into shares" << std::endl;
        exit(EXIT_FAILURE);
    }
    secretShared.insert(&s);
    return getPseudoSharesInternal(s, nbShares);
}


static Node & replaceSharesWithSecretsAndMasksRec(Node & node, std::map<Node *, Node *> & m) {
    if (node.nature != OP) {
        return node;
    }

    std::vector<Node * > children;

    for (const auto & child : *node.children) {
        if (m.contains(child)) {
            children.push_back(m[child]);
            continue;
        }
        if (child->shareOcc->size() != 0) {
            Node & newChild = replaceSharesWithSecretsAndMasksRec(*child, m);
            children.push_back(&newChild);
            continue;
        }
        children.push_back(child);
    }
    Node & n = Node::OpNode(node.op, children);
    m[&node] = &n;
    return n;
}


Node & replaceSharesWithSecretsAndMasks(Node & e) {
    std::map<Node *, Node *> m;
    for (const auto & [s, v] : *e.shareOcc) {
        for (const auto & [sh, w] : *v) {
            m[sh] = sh->pseudoShareEq;
        }
    }

    return replaceSharesWithSecretsAndMasksRec(e, m);
}


int32_t width(Node & e) {
    return e.width;
}


void verifMSICleanup() {
    for (const auto & hwe : HWElement::allHWElements) {
        delete hwe;
    }
    HWElement::allHWElements.clear();
    
    for (const auto & [s, a] : ArrayExp::allArrays) {
        delete a;
    }
    ArrayExp::allArrays.clear();
    
    secretShared.clear();
    registeredArrays.clear();
    registeredArraysByName.clear();
    registeredArraysByAddr.clear();

    for (const auto & n : Node::opNodes) {
        delete n;
    }
    Node::opNodes.clear();
    Node::cache.clear();
    for (const auto & [s, symbNode] : Node::symb2node) {
        delete symbNode;
    }
    Node::symb2node.clear();
    for (const auto & [w, m] : Node::cst2node) {
        for (const auto & [v, cstNode] : m) {
            delete cstNode;
        }
    }
    Node::cst2node.clear();
    for (const auto & [h, cstNode] : Node::bigCst2node) {
        delete cstNode;
    }
    Node::bigCst2node.clear();
    for (const auto & [s, strNode] : Node::str2node) {
        delete strNode;
    }
    Node::str2node.clear();
}





