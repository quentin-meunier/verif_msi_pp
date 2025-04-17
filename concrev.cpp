/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

// Concrete Evaluation

#include <set>
#include <vector>
#include <map>
#include <cstdlib>
#include <cassert>
#include <random>
#include <algorithm>


#include "concrev.hpp"
#include "node.hpp"
#include "utils.hpp"
#include "arrayexp.hpp"
#include "simplify.hpp"



int32_t getSymbolicBitsNum(Node & n) {
    std::set<Node *> allVars;
    for (const auto & [k, v] : *n.maskingMaskOcc) {
        allVars.insert(k);
    }
    #if SEL_MSK_W_NON_MSKNG_OCC
    for (const auto & [k, v] : *n.otherMaskOcc) {
        allVars.insert(k);
    }
    #else
    for (const auto & m : *n.otherMaskOcc) {
        allVars.insert(m);
    }
    #endif
    for (const auto & k : *n.secretVarOcc) {
        allVars.insert(k);
    }
    for (const auto & k : *n.publicVarOcc) {
        allVars.insert(k);
    }
    for (const auto & [k, v] : *n.shareOcc) {
        for (const auto & [l, w] : *v) {
            allVars.insert(l);
        }
    }

    int32_t nbBits = 0;
    for (const auto & e : allVars) {
        nbBits += e->width;
    }
    return nbBits;
}


// FIXME: propagate in verif_msi (single bit variables for enumeration)
// All variables in exps must be single bit
static void getVarsList(std::vector<Node *> & exps, std::vector<Node *> & allVarsVec) {
    std::set<Node *> allVars;
    for (const auto & e : exps) {
        for (const auto & [k, v] : *e->maskingMaskOcc) {
            allVars.insert(k);
        }
        #if SEL_MSK_W_NON_MSKNG_OCC
        for (const auto & [k, v] : *e->otherMaskOcc) {
            allVars.insert(k);
        }
        #else
        for (const auto & m : *e->otherMaskOcc) {
            allVars.insert(m);
        }
        #endif
        for (const auto & k : *e->secretVarOcc) {
            allVars.insert(k);
        }
        for (const auto & k : *e->publicVarOcc) {
            allVars.insert(k);
        }
        for (const auto & [k, v] : *e->shareOcc) {
            for (const auto & [l, w] : *v) {
                allVars.insert(l);
            }
        }
    }

    for (const auto & n : allVars) {
        //std::cout << "# Var : " << *n->symb << std::endl;
        if (n->width != 1) {
            std::cerr << "*** Internal Error: variables found for exhaustive evaluation should all be 1-bit wide" << std::endl;
            assert(false);
        }
    }

    std::copy(allVars.begin(), allVars.end(), std::back_inserter(allVarsVec));
    std::sort(allVarsVec.begin(), allVarsVec.end(), [](Node * a, Node * b) {
            return a->symb->compare(*b->symb) < 0;
    });

}


static Node & getExpValueRec(Node & node, std::map<Node *, Node *> & m, std::map<Node *, Node *> & expCache) {
    //std::cout << "getExpValue(" << node << ")" << std::endl;

    if (node.nature == SYMB) {
        if (m.contains(&node)) {
            //std::cout << "    return " << *m[&node] << std::endl;
            return *m[&node];
        }

        std::string::size_type pos = node.symb->find('#');
        if (pos != std::string::npos) {
            std::string symb = node.symb->substr(0, pos);
            Node & n = *Node::symb2node[symb];
            //std::cout << "    n : " << n << std::endl;
            int32_t bit = std::stoi(node.symb->substr(pos + 1));
            if (m.contains(&n)) {
                //std::cout << "    m contains " << n << std::endl;
                //std::cout << "    return " << Extract(bit, bit, *m[&n]) << std::endl;
                return Extract(bit, bit, *m[&n]);
            }
        }
        
        //std::cout << "*** Error: Value for symbol " << node.symb << " not specified" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (node.nature == CONST) {
        //std::cout << "    return " << node << std::endl;
        return node;
    }

    if (node.nature == STR) {
        return node;
    }

    if (expCache.contains(&node)) {
        //std::cout << "    return " << *expCache[&node] << std::endl;
        return *expCache[&node];
    }

    std::vector<Node *> newChildren;
    NodeOp op = node.op;
    for (const auto & child : *node.children) {
        newChildren.push_back(&getExpValueRec(*child, m, expCache));
    }
 
    Node * res;
    if (op == EXTRACT) {
        res = &Extract(*newChildren[0], *newChildren[1], *newChildren[2]);
    }
    else if (op == ZEXT) {
        res = &ZeroExt(*newChildren[0], *newChildren[1]);
    }
    else if (op == SEXT) {
        res = &SignExt(*newChildren[0], *newChildren[1]);
    }
    else if (op == CONCAT) {
        res = &Concat(newChildren);
    }
    else if (op == LSHR or op == SLSHR) {
        res = &LShR(*newChildren[0], *newChildren[1]);
    }
    else if (op == ASHR or op == SASHR) {
        res = &(*newChildren[0] >> *newChildren[1]);
    }
    else if (op == LSHL or op == SLSHL) {
        res = &(*newChildren[0] << *newChildren[1]);
    }
    else if (op == UMINUS) {
        res = &(-(*newChildren[0]));
    }
    else if (op == ARRAY) {
        if (ArrayExp::allArrays[*newChildren[0]->strn]->content == NULL) {
            std::cerr  << "*** Error: concrete evaluation of an array access is only possible with an initialized content" << std::endl;
            exit(EXIT_FAILURE);
        }
        res = &Const(ArrayExp::allArrays[*newChildren[0]->strn]->getContent(newChildren[1]->cst[0]), ArrayExp::allArrays[*newChildren[0]->strn]->outWidth);
    }
    else {
        int32_t width = node.width;
        int32_t nlimbs = width / 64;
        if (nlimbs * 64 != width) {
            nlimbs += 1;
        }
        uint64_t intRes[nlimbs];
        std::fill_n(intRes, nlimbs, 0);
        if (op == BAND) {
            for (int32_t i = 0; i < nlimbs - 1; i += 1) {
                intRes[i] = 0xFFFFFFFFFFFFFFFFULL;
            }
            if (width % 64 == 0) {
                intRes[nlimbs - 1] = 0xFFFFFFFFFFFFFFFFULL;
            }
            else {
                intRes[nlimbs - 1] = (1ULL << (width % 64)) - 1;
            }
        }
        else if (op == IMUL) {
            intRes[0] = 1;
        }
        else {
            intRes[0] = 0;
        }
        for (const auto & child : newChildren) {
            if (op == BXOR) {
                for (int32_t i = 0; i < nlimbs; i += 1) {
                    intRes[i] ^= child->cst[i];
                }
            }
            else if (op == BAND) {
                for (int32_t i = 0; i < nlimbs; i += 1) {
                    intRes[i] &= child->cst[i];
                }
            }
            else if (op == BOR) {
                for (int32_t i = 0; i < nlimbs; i += 1) {
                    intRes[i] |= child->cst[i];
                }
            }
            else if (op == BNOT) {
                for (int32_t i = 0; i < nlimbs - 1; i += 1) {
                    intRes[i] = ~child->cst[i];
                }
                if (width % 64 == 0) {
                    intRes[nlimbs - 1] = ~child->cst[nlimbs - 1];
                }
                else {
                    intRes[nlimbs - 1] = ((1ULL << (width % 64)) - 1) ^ child->cst[nlimbs - 1];
                }
            }
            else if (op == PLUS) {
                int carry = 0;
                for (int32_t i = 0; i < nlimbs; i += 1) {
                    intRes[i] += child->cst[i] + carry;
                    carry = intRes[i] < child->cst[i] ? 1 : 0;
                }
                if (width % 64 != 0) {
                    intRes[nlimbs - 1] = intRes[nlimbs - 1] & ((1ULL << (width % 64)) - 1);
                }
            }
            else if (op == IMUL) {
                assert(nlimbs == 1);
                intRes[0] = width == 64 ? (intRes[0] * child->cst[0]) : (intRes[0] * child->cst[0]) & ((1ULL << width) - 1);
            }
            else {
                assert(false);
            }
        }
        res = &Const(intRes, nlimbs, width);
    }

    expCache[&node] = res;
    //std::cout << "    return " << *res << std::endl;
    return *res;
}



static Node & getExpValue(Node & node, std::map<Node *, Node *> & m) {
    std::map<Node *, Node *> expCache;
    return getExpValueRec(node, m, expCache);
}



static bool compareExpsWithExevRec(Node & e0, Node & e1, std::vector<Node *> & allVars, int32_t idx, std::map<Node *, Node *> & m, std::map<Node *, Node *> & res, uint64_t * v0, uint64_t * v1) {
    if (idx < (int32_t) allVars.size()) {
        Node & var = *allVars[idx];
        for (int64_t val = 0; val < (1LL << var.width); val += 1) {
            m[&var] = &Const(val, var.width);
            //std::cout << var << " = " << *m[&var] << std::endl;
            bool retval = compareExpsWithExevRec(e0, e1, allVars, idx + 1, m, res, v0, v1);
            if (!retval) {
                return false;
            }
            m[&var] = NULL;
        }
        return true;
    }
    else {
        Node & n0 = getExpValue(e0, m);
        //std::cout << "n0 : " << n0 << std::endl;
        Node & n1 = getExpValue(e1, m);
        //std::cout << "n1 : " << n1 << std::endl;
        if (equivalence(n0, n1)) {
            return true;
        }
        else {
            res = m;
            for (int32_t i = 0; i < n0.nlimbs; i += 1) {
                v0[i] = n0.cst[i];
                v1[i] = n1.cst[i];
            }
            return false;
        }
    }
}

 

bool compareExpsWithExev(Node & e0, Node & e1) {
    if (e0.width != e1.width) {
        return false;
    }
    assert(e0.width <= 64);
    int32_t nlimbs = e0.width / 64;
    if (nlimbs * 64 != e0.width) {
        nlimbs += 1;
    }
    std::map<Node *, Node *> res;
    uint64_t v0[nlimbs];
    uint64_t v1[nlimbs];
    return compareExpsWithExev(e0, e1, res, v0, v1);
}


bool compareExpsWithExev(Node & e0, Node & e1, std::map<Node *, Node *> & res, uint64_t * v0, uint64_t * v1) {
    if (e0.width != e1.width) {
        return false;
    }
    assert(e0.width <= 64);
    
    Node & e0r = replaceSharesWithSecretsAndMasks(e0);
    Node & e1r = replaceSharesWithSecretsAndMasks(e1);

    Node & e0b = getBitDecomposition(e0r);
    Node & e1b = getBitDecomposition(e1r);

    std::vector<Node *> exps {&e0b, &e1b};
    std::vector<Node *> allVarsVec;
    getVarsList(exps, allVarsVec);
    std::map<Node *, Node *> m;
    compareExpsWithExevRec(e0b, e1b, allVarsVec, 0, m, res, v0, v1);
    return res.size() == 0;
}


bool compareExpsWithRandev(Node & e0, Node & e1, int32_t nbEval) {
    if (e0.width != e1.width) {
        return false;
    }
    std::map<Node *, Node *> res;
    int32_t nlimbs = e0.width / 64;
    if (nlimbs * 64 != e0.width) {
        nlimbs += 1;
    }
    uint64_t v0[nlimbs];
    uint64_t v1[nlimbs];
    return compareExpsWithRandev(e0, e1, nbEval, res, v0, v1);
}


bool compareExpsWithRandev(Node & e0, Node & e1, int32_t nbEval, std::map<Node *, Node *> & res, uint64_t * v0, uint64_t * v1) {
    // Random Evaluations

    //std::cout << "e0 : " << e0 << std::endl;
    //std::cout << "e1 : " << e1 << std::endl;
    Node & e0r = replaceSharesWithSecretsAndMasks(e0);
    Node & e1r = replaceSharesWithSecretsAndMasks(e1);

    Node & e0b = getBitDecomposition(e0r);
    Node & e1b = getBitDecomposition(e1r);

    std::vector<Node *> exps {&e0b, &e1b};
    std::vector<Node *> allVarsVec;
    getVarsList(exps, allVarsVec);
    std::map<Node *, Node *> m;

    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    for (int32_t i = 0; i < nbEval; i += 1) {
        m.clear();
        for (const auto & v : allVarsVec) {
            std::uniform_int_distribution<> distrib(0, (1ULL << v->width) - 1);
            m.insert_or_assign(v, &Const(distrib(gen), v->width));
            //std::cout << *v << " <- " << *m[v] << std::endl;
        }

        Node & n0 = getExpValue(e0b, m);
        //std::cout << "n0 : " << n0 << " (width = " << n0.width << ")" << std::endl;
        Node & n1 = getExpValue(e1b, m);
        //std::cout << "n1 : " << n1 << " (width = " << n1.width << ")" << std::endl;
        if (!equivalence(n0, n1)) {
            res = m;
            for (int32_t i = 0; i < n0.nlimbs; i += 1) {
                v0[i] = n0.cst[i];
                v1[i] = n1.cst[i];
            }
            return false;
        }
    }
    return true;
}


static void getDistribRefBis(Node & e0, std::map<uint64_t, uint64_t> & distribRef, std::vector<Node *> & nonSecretVars, int32_t idx, std::map<Node *, Node *> & m) {
    if (idx < (int32_t) nonSecretVars.size()) {
        Node & var = *nonSecretVars[idx];
        for (int64_t val = 0; val < (1LL << var.width); val += 1) {
            m.insert_or_assign(&var, &Const(val, var.width));
            getDistribRefBis(e0, distribRef, nonSecretVars, idx + 1, m);
        }
    }
    else {
        uint64_t v = getExpValue(e0, m).cst[0];
        distribRef[v] += 1;
    }
}


static void getDistribRef(Node & e0, std::vector<Node *> & secretVars, std::vector<Node *> & nonSecretVars, std::map<uint64_t, uint64_t> & distribRef) {
    std::map<Node *, Node *> m;
    for (const auto & k : secretVars) {
        m[k] = &Const(0, k->width);
    }

    for (int64_t v = 0; v < (1LL << e0.width); v += 1) {
        distribRef.insert_or_assign(v, 0);
    }
    getDistribRefBis(e0, distribRef, nonSecretVars, 0, m);
}


static void getDistribWithExevRecBis(Node & e0, std::map<uint64_t, uint64_t> & distrib, std::map<uint64_t, uint64_t> & distribRef, std::vector<Node *> & nonSecretVars, int32_t idx, std::map<Node *, Node *> & m) {
    if (idx < (int32_t) nonSecretVars.size()) {
        Node & var = *nonSecretVars[idx];
        for (int64_t val = 0; val < (1LL << var.width); val += 1) {
            m.insert_or_assign(&var, &Const(val, var.width));
            getDistribWithExevRecBis(e0, distrib, distribRef, nonSecretVars, idx + 1, m);
        }
    }
    else {
        uint64_t v = getExpValue(e0, m).cst[0];
        distrib[v] += 1;
    }
}


static bool getDistribWithExevRec(Node & e0, std::map<uint64_t, uint64_t> & distribRef, std::vector<Node *> & secretVars, std::vector<Node *> & nonSecretVars, int32_t idx, std::map<Node *, Node *> & m, bool * rud) {
    if (idx < (int32_t) secretVars.size()) {
        Node & var = *secretVars[idx];
        for (int64_t val = 0; val < (1LL << var.width); val += 1) {
            m.insert_or_assign(&var, &Const(val, var.width));
            bool sid = getDistribWithExevRec(e0, distribRef, secretVars, nonSecretVars, idx + 1, m, rud);
            if (!sid) {
                *rud = false;
                return false;
            }
        }
        return true;
    }
    else {
        std::map<uint64_t, uint64_t> distrib;
        for (int64_t v = 0; v < (1LL << e0.width); v += 1) {
            distrib.insert_or_assign(v, 0);
        }
        getDistribWithExevRecBis(e0, distrib, distribRef, nonSecretVars, 0, m);
        *rud = true;
        for (int64_t v = 0; v < (1LL << e0.width); v += 1) {
            if (distrib[v] != distribRef[0]) {
                *rud = false;
            }
            if (distrib[v] != distribRef[v]) {
                return false;
            }
        }
        return true;
    }
}


bool getDistribWithExev(Node & e, bool * rud) {
    assert(e.width <= 64);

    Node & e1 = replaceSharesWithSecretsAndMasks(e);
    Node & e0 = getBitDecomposition(e1);
    
    std::vector<Node *> allVarsVec;
    std::vector<Node *> exp {&e0};
    getVarsList(exp, allVarsVec);

    std::vector<Node *> secretVars;
    std::vector<Node *> nonSecretVars;

    for (const auto & v : allVarsVec) {
        if (v->symbType == 'S' or v->symbType == 'P') {
            secretVars.push_back(v);
        }
        else {
            nonSecretVars.push_back(v);
        }
    }

    std::map<uint64_t, uint64_t> distribRef;
    getDistribRef(e0, secretVars, nonSecretVars, distribRef);
    
    std::map<Node *, Node *> m;
    return getDistribWithExevRec(e0, distribRef, secretVars, nonSecretVars, 0, m, rud);
}


bool getDistribWithExev(Node & e) {
    bool rud;
    return getDistribWithExev(e, &rud);
}


