/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

// Concrete Evaluation

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "arrayexp.hpp"
#include "concrev.hpp"
#include "node.hpp"
#include "simplify.hpp"
#include "utils.hpp"
#include "utils_private.hpp"


namespace vmsi {


int32_t getSymbolicBitsNum(Node & n) {
    std::set<Node *> allVars;
    for (const auto & [k, v] : n.maskingMaskOcc) {
        allVars.insert(k);
    }
#if SEL_MSK_W_NON_MSKNG_OCC
    for (const auto & [k, v] : n.otherMaskOcc) {
        allVars.insert(k);
    }
#else
    for (const auto & m : n.otherMaskOcc) {
        allVars.insert(m);
    }
#endif
    for (const auto & k : n.secretVarOcc) {
        allVars.insert(k);
    }
    for (const auto & k : n.publicVarOcc) {
        allVars.insert(k);
    }
    for (const auto & [k, v] : n.shareOcc) {
        for (const auto & [l, w] : v) {
            allVars.insert(l);
        }
    }

    int32_t nbBits = 0;
    for (const auto & e : allVars) {
        nbBits += e->width;
    }
    return nbBits;
}


int32_t getMaxSymbVarWidth(Node & n) {
    std::set<Node *> allVars;
    for (const auto & [k, v] : n.maskingMaskOcc) {
        allVars.insert(k);
    }
#if SEL_MSK_W_NON_MSKNG_OCC
    for (const auto & [k, v] : n.otherMaskOcc) {
        allVars.insert(k);
    }
#else
    for (const auto & m : n.otherMaskOcc) {
        allVars.insert(m);
    }
#endif
    for (const auto & k : n.secretVarOcc) {
        allVars.insert(k);
    }
    for (const auto & k : n.publicVarOcc) {
        allVars.insert(k);
    }
    for (const auto & [k, v] : n.shareOcc) {
        for (const auto & [l, w] : v) {
            allVars.insert(l);
        }
    }

    int32_t maxNbBits = 0;
    for (const auto & e : allVars) {
        if (e->width > maxNbBits) {
            maxNbBits = e->width;
        }
    }
    return maxNbBits;
}


// All variables in exps must be single bit
static void getVarsList(std::vector<Node *> & exps, std::vector<Node *> & allVarsVec) {
    std::set<Node *> allVars;
    for (const auto & e : exps) {
        for (const auto & [k, v] : e->maskingMaskOcc) {
            allVars.insert(k);
        }
#if SEL_MSK_W_NON_MSKNG_OCC
        for (const auto & [k, v] : e->otherMaskOcc) {
            allVars.insert(k);
        }
#else
        for (const auto & m : e->otherMaskOcc) {
            allVars.insert(m);
        }
#endif
        for (const auto & k : e->secretVarOcc) {
            allVars.insert(k);
        }
        for (const auto & k : e->publicVarOcc) {
            allVars.insert(k);
        }
        for (const auto & [k, v] : e->shareOcc) {
            for (const auto & [l, w] : v) {
                allVars.insert(l);
            }
        }
    }

    for (const auto & n : allVars) {
        if (n->width != 1) {
            std::cerr << "*** Internal Error: variables found for exhaustive evaluation should all be 1-bit wide" << std::endl;
            assert(false);
        }
    }

    std::copy(allVars.begin(), allVars.end(), std::back_inserter(allVarsVec));
    std::sort(allVarsVec.begin(), allVarsVec.end(), [](Node * a, Node * b) { return a->symb->compare(*b->symb) < 0; });
}


static Node & getExpValueRec(Node & node, std::map<Node *, Node *> & m, std::map<Node *, Node *> & expCache) {
    if (node.nature == SYMB) {
        if (m.contains(&node)) {
            return *m[&node];
        }

        std::string::size_type pos = node.symb->find('#');
        if (pos != std::string::npos) {
            std::string symb = node.symb->substr(0, pos);
            Node & n = *Node::symb2node[symb];
            int32_t bit = std::stoi(node.symb->substr(pos + 1));
            if (m.contains(&n)) {
                return Extract(bit, bit, *m[&n]);
            }
        }

        std::cout << "*** Error: Value for symbol " << node << " not specified" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (node.nature == CONST) {
        return node;
    }

    if (node.nature == STR) {
        return node;
    }

    if (expCache.contains(&node)) {
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
            std::cerr << "*** Error: concrete evaluation of an array access is only "
                         "possible with an initialized content"
                      << std::endl;
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
        else if (op == IMUL || op == GMUL) {
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
            else if (op == GMUL) {
                assert(width == 8);
                intRes[0] = gmulInt(intRes[0], child->cst[0]);
            }
            else {
                assert(false);
            }
        }
        res = &Const(intRes, nlimbs, width);
    }

    expCache[&node] = res;
    return *res;
}


Node & getExpValue(Node & node, std::map<Node *, Node *> & m) {
    std::map<Node *, Node *> expCache;
    return getExpValueRec(node, m, expCache);
}


// Fast topological instruction set for expressions <= 64 bits
enum class FastOp : uint8_t {
    VAR,
    CONST,
    BXOR,
    BAND,
    BOR,
    BNOT,
    LSHR,
    LSHL,
    ASHR,
    PLUS,
    IMUL,
    GMUL,
    UMINUS,
    EXTRACT,
    ZEXT,
    SEXT,
    CONCAT,
    ARRAY
};


struct FastInstr {
    FastOp op;
    int32_t outReg = -1;
    int32_t inReg1 = -1;
    int32_t inReg2 = -1;
    int32_t width = 0;
    uint64_t cstVal = 0;
    const std::string * arrayName = nullptr;
    std::vector<int32_t> multiIn;
    std::vector<int32_t> childWidths;
};


class FastCircuit {
  public:
    std::vector<FastInstr> code;
    std::map<Node *, int32_t> var2reg;
    std::map<Node *, int32_t> node2reg;
    int32_t numRegs = 0;
    int32_t outReg = -1;
    bool isBitsliceable = true;

    int32_t compileNode(Node & n) {
        auto it = node2reg.find(&n);
        if (it != node2reg.end()) {
            return it->second;
        }

        if (n.nature == CONST) {
            int32_t r = numRegs++;
            node2reg[&n] = r;
            FastInstr instr;
            instr.op = FastOp::CONST;
            instr.outReg = r;
            instr.width = n.width;
            instr.cstVal = (n.nlimbs > 1) ? n.cst[0] : n.inlineCst;
            code.push_back(instr);
            return r;
        }

        if (n.nature == STR) {
            int32_t r = numRegs++;
            node2reg[&n] = r;
            FastInstr instr;
            instr.op = FastOp::CONST;
            instr.outReg = r;
            instr.width = 0;
            instr.cstVal = 0;
            instr.arrayName = n.strn;
            code.push_back(instr);
            isBitsliceable = false;
            return r;
        }

        if (n.nature == SYMB) {
            auto itv = var2reg.find(&n);
            if (itv != var2reg.end()) {
                node2reg[&n] = itv->second;
                return itv->second;
            }
            int32_t r = numRegs++;
            var2reg[&n] = r;
            node2reg[&n] = r;
            FastInstr instr;
            instr.op = FastOp::VAR;
            instr.outReg = r;
            instr.width = n.width;
            code.push_back(instr);
            return r;
        }

        std::vector<int32_t> childRegs;
        std::vector<int32_t> childWidths;
        if (n.children != nullptr) {
            for (Node * c : *n.children) {
                childRegs.push_back(compileNode(*c));
                childWidths.push_back(c->width);
            }
        }

        int32_t r = numRegs++;
        node2reg[&n] = r;
        FastInstr instr;
        instr.outReg = r;
        instr.width = n.width;

        switch (n.op) {
        case BXOR:
            instr.op = FastOp::BXOR;
            instr.multiIn = std::move(childRegs);
            break;
        case BAND:
            instr.op = FastOp::BAND;
            instr.multiIn = std::move(childRegs);
            break;
        case BOR:
            instr.op = FastOp::BOR;
            instr.multiIn = std::move(childRegs);
            break;
        case BNOT:
            instr.op = FastOp::BNOT;
            instr.inReg1 = childRegs[0];
            break;
        case LSHR:
        case SLSHR:
            instr.op = FastOp::LSHR;
            instr.inReg1 = childRegs[0];
            instr.inReg2 = childRegs[1];
            isBitsliceable = false;
            break;
        case LSHL:
        case SLSHL:
            instr.op = FastOp::LSHL;
            instr.inReg1 = childRegs[0];
            instr.inReg2 = childRegs[1];
            isBitsliceable = false;
            break;
        case ASHR:
        case SASHR:
            instr.op = FastOp::ASHR;
            instr.inReg1 = childRegs[0];
            instr.inReg2 = childRegs[1];
            isBitsliceable = false;
            break;
        case PLUS:
            instr.op = FastOp::PLUS;
            instr.multiIn = std::move(childRegs);
            isBitsliceable = false;
            break;
        case IMUL:
            instr.op = FastOp::IMUL;
            instr.multiIn = std::move(childRegs);
            isBitsliceable = false;
            break;
        case GMUL:
            instr.op = FastOp::GMUL;
            instr.multiIn = std::move(childRegs);
            isBitsliceable = false;
            break;
        case UMINUS:
            instr.op = FastOp::UMINUS;
            instr.inReg1 = childRegs[0];
            isBitsliceable = false;
            break;
        case EXTRACT:
            instr.op = FastOp::EXTRACT;
            instr.inReg1 = childRegs[0];
            instr.inReg2 = childRegs[1];
            instr.multiIn = {childRegs[2]};
            if (n.width != 1 || childWidths[2] != 1) {
                isBitsliceable = false;
            }
            break;
        case ZEXT:
            instr.op = FastOp::ZEXT;
            instr.inReg1 = childRegs[0];
            instr.inReg2 = childRegs[1];
            isBitsliceable = false;
            break;
        case SEXT:
            instr.op = FastOp::SEXT;
            instr.inReg1 = childRegs[0];
            instr.inReg2 = childRegs[1];
            isBitsliceable = false;
            break;
        case CONCAT:
            instr.op = FastOp::CONCAT;
            instr.multiIn = std::move(childRegs);
            instr.childWidths = std::move(childWidths);
            isBitsliceable = false;
            break;
        case ARRAY:
            instr.op = FastOp::ARRAY;
            instr.inReg1 = childRegs[0];
            instr.inReg2 = childRegs[1];
            instr.arrayName = (*n.children)[0]->strn;
            isBitsliceable = false;
            break;
        default:
            std::cerr << "Unsupported op: " << n.op << std::endl;
            assert(false);
        }
        code.push_back(instr);
        return r;
    }


    void compile(Node & root) {
        code.clear();
        var2reg.clear();
        node2reg.clear();
        numRegs = 0;
        isBitsliceable = (root.width == 1);
        outReg = compileNode(root);
    }


    inline uint64_t eval_scalar(uint64_t * regs) const {
        for (const auto & instr : code) {
            switch (instr.op) {
            case FastOp::VAR:
                break;
            case FastOp::CONST:
                regs[instr.outReg] = instr.cstVal;
                break;
            case FastOp::BXOR: {
                uint64_t val = 0;
                for (int32_t inR : instr.multiIn) {
                    val ^= regs[inR];
                }
                regs[instr.outReg] = val;
                break;
            }
            case FastOp::BAND: {
                uint64_t val = (instr.width == 64) ? ~0ULL : ((1ULL << instr.width) - 1);
                for (int32_t inR : instr.multiIn) {
                    val &= regs[inR];
                }
                regs[instr.outReg] = val;
                break;
            }
            case FastOp::BOR: {
                uint64_t val = 0;
                for (int32_t inR : instr.multiIn) {
                    val |= regs[inR];
                }
                regs[instr.outReg] = val;
                break;
            }
            case FastOp::BNOT: {
                uint64_t mask = (instr.width == 64) ? ~0ULL : ((1ULL << instr.width) - 1);
                regs[instr.outReg] = (~regs[instr.inReg1]) & mask;
                break;
            }
            case FastOp::LSHR:
                regs[instr.outReg] = regs[instr.inReg1] >> regs[instr.inReg2];
                break;
            case FastOp::LSHL: {
                uint64_t mask = (instr.width == 64) ? ~0ULL : ((1ULL << instr.width) - 1);
                regs[instr.outReg] = (regs[instr.inReg1] << regs[instr.inReg2]) & mask;
                break;
            }
            case FastOp::ASHR: {
                uint64_t v = regs[instr.inReg1];
                int32_t sh = regs[instr.inReg2];
                int32_t w = instr.width;
                bool sign = (v >> (w - 1)) & 1;
                uint64_t res = v >> sh;
                if (sign && sh > 0) {
                    uint64_t signBits = (sh >= 64) ? ~0ULL : ((1ULL << sh) - 1);
                    res |= (signBits << (w - sh));
                }
                uint64_t mask = (w == 64) ? ~0ULL : ((1ULL << w) - 1);
                regs[instr.outReg] = res & mask;
                break;
            }
            case FastOp::PLUS: {
                uint64_t sum = 0;
                for (int32_t inR : instr.multiIn) {
                    sum += regs[inR];
                }
                uint64_t mask = (instr.width == 64) ? ~0ULL : ((1ULL << instr.width) - 1);
                regs[instr.outReg] = sum & mask;
                break;
            }
            case FastOp::IMUL: {
                uint64_t prod = 1;
                for (int32_t inR : instr.multiIn) {
                    prod *= regs[inR];
                }
                uint64_t mask = (instr.width == 64) ? ~0ULL : ((1ULL << instr.width) - 1);
                regs[instr.outReg] = prod & mask;
                break;
            }
            case FastOp::GMUL: {
                uint64_t prod = 1;
                for (int32_t inR : instr.multiIn) {
                    prod = gmulInt(prod, regs[inR]);
                }
                regs[instr.outReg] = prod & 0xFF;
                break;
            }
            case FastOp::UMINUS: {
                uint64_t mask = (instr.width == 64) ? ~0ULL : ((1ULL << instr.width) - 1);
                regs[instr.outReg] = (-regs[instr.inReg1]) & mask;
                break;
            }
            case FastOp::EXTRACT: {
                uint64_t high = regs[instr.inReg1];
                uint64_t low = regs[instr.inReg2];
                uint64_t val = regs[instr.multiIn[0]];
                uint64_t w = high - low + 1;
                uint64_t mask = (w == 64) ? ~0ULL : ((1ULL << w) - 1);
                regs[instr.outReg] = (val >> low) & mask;
                break;
            }
            case FastOp::ZEXT:
                regs[instr.outReg] = regs[instr.inReg2];
                break;
            case FastOp::SEXT: {
                uint64_t val = regs[instr.inReg2];
                int32_t origW = instr.inReg1;
                bool sign = (val >> (origW - 1)) & 1;
                if (sign) {
                    uint64_t top = (1ULL << instr.width) - (1ULL << origW);
                    val |= top;
                }
                regs[instr.outReg] = val;
                break;
            }
            case FastOp::CONCAT: {
                uint64_t val = 0;
                int32_t shift = 0;
                for (int i = (int) instr.multiIn.size() - 1; i >= 0; --i) {
                    int32_t inR = instr.multiIn[i];
                    int32_t w = instr.childWidths[i];
                    uint64_t mask = (w == 64) ? ~0ULL : ((1ULL << w) - 1);
                    val |= ((regs[inR] & mask) << shift);
                    shift += w;
                }
                regs[instr.outReg] = val;
                break;
            }
            case FastOp::ARRAY: {
                auto & arr = ArrayExp::allArrays[*instr.arrayName];
                regs[instr.outReg] = arr->getContent(regs[instr.inReg2]);
                break;
            }
            }
        }
        return regs[outReg];
    }


    inline uint64_t eval_bitsliced(uint64_t * regs) const {
        for (const auto & instr : code) {
            switch (instr.op) {
            case FastOp::VAR:
                break;
            case FastOp::CONST:
                regs[instr.outReg] = instr.cstVal ? ~0ULL : 0ULL;
                break;
            case FastOp::BXOR: {
                uint64_t v = 0;
                for (int32_t inR : instr.multiIn) {
                    v ^= regs[inR];
                }
                regs[instr.outReg] = v;
                break;
            }
            case FastOp::BAND: {
                uint64_t v = ~0ULL;
                for (int32_t inR : instr.multiIn) {
                    v &= regs[inR];
                }
                regs[instr.outReg] = v;
                break;
            }
            case FastOp::BOR: {
                uint64_t v = 0;
                for (int32_t inR : instr.multiIn) {
                    v |= regs[inR];
                }
                regs[instr.outReg] = v;
                break;
            }
            case FastOp::BNOT:
                regs[instr.outReg] = ~regs[instr.inReg1];
                break;
            case FastOp::EXTRACT:
                regs[instr.outReg] = regs[instr.multiIn[0]];
                break;
            default:
                assert(false);
            }
        }
        return regs[outReg];
    }
};


static void fastGetDistrib(const FastCircuit & fc, const std::vector<int32_t> & maskRegs, uint64_t * regs, std::vector<uint64_t> & distrib, int32_t outWidth) {
    int32_t numMasks = (int32_t) maskRegs.size();
    if (fc.isBitsliceable && outWidth == 1) {
        distrib.assign(2, 0);
        const uint64_t PATTERNS[6] = {0xAAAAAAAAAAAAAAAAULL, 0xCCCCCCCCCCCCCCCCULL,
                                      0xF0F0F0F0F0F0F0F0ULL, 0xFF00FF00FF00FF00ULL,
                                      0xFFFF0000FFFF0000ULL, 0xFFFFFFFF00000000ULL};

        if (numMasks == 0) {
            uint64_t out = fc.eval_bitsliced(regs);
            if (out & 1) {
                distrib[1] = 1;
            }
            else {
                distrib[0] = 1;
            }
            return;
        }

        if (numMasks <= 6) {
            uint64_t activeCount = 1ULL << numMasks;
            uint64_t activeMask = (activeCount == 64) ? ~0ULL : ((1ULL << activeCount) - 1);
            for (int32_t i = 0; i < numMasks; i += 1) {
                regs[maskRegs[i]] = PATTERNS[i] & activeMask;
            }
            uint64_t out = fc.eval_bitsliced(regs) & activeMask;
            uint64_t ones = (uint64_t) std::popcount(out);
            distrib[1] = ones;
            distrib[0] = activeCount - ones;
            return;
        }

        uint64_t numBlocks = 1ULL << (numMasks - 6);
        for (int32_t i = 0; i < 6; i += 1) {
            regs[maskRegs[i]] = PATTERNS[i];
        }
        for (uint64_t b = 0; b < numBlocks; b += 1) {
            for (int32_t i = 6; i < numMasks; i += 1) {
                regs[maskRegs[i]] = ((b >> (i - 6)) & 1ULL) ? ~0ULL : 0ULL;
            }
            uint64_t out = fc.eval_bitsliced(regs);
            uint64_t ones = (uint64_t) std::popcount(out);
            distrib[1] += ones;
            distrib[0] += (64 - ones);
        }
        return;
    }

    // Scalar fallback
    uint64_t total = 1ULL << numMasks;
    distrib.assign(1ULL << outWidth, 0);
    for (uint64_t v = 0; v < total; v += 1) {
        for (int32_t i = 0; i < numMasks; i += 1) {
            regs[maskRegs[i]] = (v >> i) & 1ULL;
        }
        uint64_t out = fc.eval_scalar(regs);
        distrib[out] += 1;
    }
}


static bool fastCompareExpsWithExevRec(const FastCircuit & fc0, const FastCircuit & fc1, const std::vector<int32_t> & varRegs0, const std::vector<int32_t> & varRegs1, const std::vector<Node *> & allVars, int32_t idx, uint64_t * regs0, uint64_t * regs1, std::map<Node *, Node *> & res, uint64_t * v0, uint64_t * v1) {
    if (idx < (int32_t) allVars.size()) {
        Node & var = *allVars[idx];
        int32_t r0 = varRegs0[idx];
        int32_t r1 = varRegs1[idx];
        for (int64_t val = 0; val < (1LL << var.width); val += 1) {
            if (r0 >= 0) {
                regs0[r0] = val;
            }
            if (r1 >= 0) {
                regs1[r1] = val;
            }
            bool retval = fastCompareExpsWithExevRec(fc0, fc1, varRegs0, varRegs1, allVars, idx + 1, regs0, regs1, res, v0, v1);
            if (!retval) {
                return false;
            }
        }
        return true;
    }
    else {
        uint64_t val0 = fc0.eval_scalar(regs0);
        uint64_t val1 = fc1.eval_scalar(regs1);
        if (val0 == val1) {
            return true;
        }
        else {
            for (size_t i = 0; i < allVars.size(); i += 1) {
                uint64_t v = (varRegs0[i] >= 0) ? regs0[varRegs0[i]] : regs1[varRegs1[i]];
                res[allVars[i]] = &Const(v, allVars[i]->width);
            }
            v0[0] = val0;
            v1[0] = val1;
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

    std::vector<Node *> exps{&e0b, &e1b};
    std::vector<Node *> allVarsVec;
    getVarsList(exps, allVarsVec);

    FastCircuit fc0;
    FastCircuit fc1;
    fc0.compile(e0b);
    fc1.compile(e1b);

    std::vector<int32_t> varRegs0;
    std::vector<int32_t> varRegs1;
    for (Node * v : allVarsVec) {
        varRegs0.push_back(fc0.var2reg.contains(v) ? fc0.var2reg[v] : -1);
        varRegs1.push_back(fc1.var2reg.contains(v) ? fc1.var2reg[v] : -1);
    }

    std::vector<uint64_t> regs0(fc0.numRegs, 0);
    std::vector<uint64_t> regs1(fc1.numRegs, 0);

    fastCompareExpsWithExevRec(fc0, fc1, varRegs0, varRegs1, allVarsVec, 0, regs0.data(), regs1.data(), res, v0, v1);
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
    Node & e0r = replaceSharesWithSecretsAndMasks(e0);
    Node & e1r = replaceSharesWithSecretsAndMasks(e1);

    Node & e0b = getBitDecomposition(e0r);
    Node & e1b = getBitDecomposition(e1r);

    std::vector<Node *> exps{&e0b, &e1b};
    std::vector<Node *> allVarsVec;
    getVarsList(exps, allVarsVec);

    FastCircuit fc0;
    FastCircuit fc1;
    fc0.compile(e0b);
    fc1.compile(e1b);

    std::vector<int32_t> varRegs0;
    std::vector<int32_t> varRegs1;
    for (Node * v : allVarsVec) {
        varRegs0.push_back(fc0.var2reg.contains(v) ? fc0.var2reg[v] : -1);
        varRegs1.push_back(fc1.var2reg.contains(v) ? fc1.var2reg[v] : -1);
    }

    std::vector<uint64_t> regs0(fc0.numRegs, 0);
    std::vector<uint64_t> regs1(fc1.numRegs, 0);

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int32_t i = 0; i < nbEval; i += 1) {
        for (size_t vi = 0; vi < allVarsVec.size(); vi += 1) {
            uint64_t val = gen() & 1ULL;
            if (varRegs0[vi] >= 0) {
                regs0[varRegs0[vi]] = val;
            }
            if (varRegs1[vi] >= 0) {
                regs1[varRegs1[vi]] = val;
            }
        }

        uint64_t val0 = fc0.eval_scalar(regs0.data());
        uint64_t val1 = fc1.eval_scalar(regs1.data());
        if (val0 != val1) {
            res.clear();
            for (size_t vi = 0; vi < allVarsVec.size(); vi += 1) {
                uint64_t v = (varRegs0[vi] >= 0) ? regs0[varRegs0[vi]] : regs1[varRegs1[vi]];
                res[allVarsVec[vi]] = &Const(v, allVarsVec[vi]->width);
            }
            v0[0] = val0;
            v1[0] = val1;
            return false;
        }
    }
    return true;
}


static bool fastGetDistribWithExevRec(const FastCircuit & fc, const std::vector<uint64_t> & distribRef, const std::vector<int32_t> & secretRegs, const std::vector<int32_t> & maskRegs, int32_t idx, uint64_t * regs, int32_t outWidth, bool * rud) {
    if (idx < (int32_t) secretRegs.size()) {
        int32_t r = secretRegs[idx];
        if (r >= 0) {
            regs[r] = 0;
            if (!fastGetDistribWithExevRec(fc, distribRef, secretRegs, maskRegs, idx + 1, regs, outWidth, rud)) {
                return false;
            }
            regs[r] = (fc.isBitsliceable && outWidth == 1) ? ~0ULL : 1ULL;
            return fastGetDistribWithExevRec(fc, distribRef, secretRegs, maskRegs, idx + 1, regs, outWidth, rud);
        }
        else {
            return fastGetDistribWithExevRec(fc, distribRef, secretRegs, maskRegs, idx + 1, regs, outWidth, rud);
        }
    }
    else {
        std::vector<uint64_t> distrib;
        fastGetDistrib(fc, maskRegs, regs, distrib, outWidth);
        *rud = true;
        for (size_t v = 0; v < distrib.size(); v += 1) {
            if (distrib[v] != distribRef[0]) {
                *rud = false;
            }
            if (distrib[v] != distribRef[v]) {
                *rud = false;
                return false;
            }
        }
        return true;
    }
}


static bool fastEnumeratePublicVarsRec(const FastCircuit & fc, const std::vector<int32_t> & publicRegs, const std::vector<int32_t> & secretRegs, const std::vector<int32_t> & maskRegs, int32_t idx, uint64_t * regs, int32_t outWidth, bool * rud) {
    if (idx < (int32_t) publicRegs.size()) {
        int32_t r = publicRegs[idx];
        if (r >= 0) {
            regs[r] = 0;
            if (!fastEnumeratePublicVarsRec(fc, publicRegs, secretRegs, maskRegs, idx + 1, regs, outWidth, rud)) {
                return false;
            }
            regs[r] = (fc.isBitsliceable && outWidth == 1) ? ~0ULL : 1ULL;
            return fastEnumeratePublicVarsRec(fc, publicRegs, secretRegs, maskRegs, idx + 1, regs, outWidth, rud);
        }
        else {
            return fastEnumeratePublicVarsRec(fc, publicRegs, secretRegs, maskRegs, idx + 1, regs, outWidth, rud);
        }
    }
    else {
        // Compute reference distribution with secrets = 0
        for (int32_t sr : secretRegs) {
            if (sr >= 0) {
                regs[sr] = 0;
            }
        }
        std::vector<uint64_t> distribRef;
        fastGetDistrib(fc, maskRegs, regs, distribRef, outWidth);

        bool rudLocal;
        bool res = fastGetDistribWithExevRec(fc, distribRef, secretRegs, maskRegs, 0, regs, outWidth, &rudLocal);
        if (!rudLocal || !res) {
            *rud = false;
        }
        return res;
    }
}


bool getDistribWithExev(Node & e, bool * rud) {
    assert(e.width <= 64);

    secPropValidity(e, TPS);
    Node & e0 = getBitDecomposition(e);

    std::vector<Node *> allVarsVec;
    std::vector<Node *> exp{&e0};
    getVarsList(exp, allVarsVec);

    FastCircuit fc;
    fc.compile(e0);

    std::vector<int32_t> secretRegs;
    std::vector<int32_t> publicRegs;
    std::vector<int32_t> maskRegs;

    for (const auto & v : allVarsVec) {
        int32_t r = fc.var2reg.contains(v) ? fc.var2reg[v] : -1;
        if (v->symbType == 'S') {
            if (r >= 0) {
                secretRegs.push_back(r);
            }
        }
        else if (v->symbType == 'P') {
            if (r >= 0) {
                publicRegs.push_back(r);
            }
        }
        else {
            assert(v->symbType == 'M');
            if (r >= 0) {
                maskRegs.push_back(r);
            }
        }
    }

    *rud = true;
    std::vector<uint64_t> regs(fc.numRegs, 0);
    return fastEnumeratePublicVarsRec(fc, publicRegs, secretRegs, maskRegs, 0, regs.data(), e0.width, rud);
}


bool isTPSWithExev(Node & e) {
    bool rud = true;
    return getDistribWithExev(e, &rud);
}


// Returns true if the share s is effective, i.e. if it has no impact on the exp
// distribution, false otherwise
static bool fastShareIsEffective(const FastCircuit & fc, const std::vector<int32_t> & sharesWithFixedValRegs, int32_t sReg, const std::vector<int32_t> & maskRegs, int32_t idx, uint64_t * regs, int32_t outWidth) {
    if (idx < (int32_t) sharesWithFixedValRegs.size()) {
        int32_t r = sharesWithFixedValRegs[idx];
        regs[r] = 0;
        if (fastShareIsEffective(fc, sharesWithFixedValRegs, sReg, maskRegs, idx + 1, regs, outWidth)) {
            return true;
        }
        regs[r] = (fc.isBitsliceable && outWidth == 1) ? ~0ULL : 1ULL;
        return fastShareIsEffective(fc, sharesWithFixedValRegs, sReg, maskRegs, idx + 1, regs, outWidth);
    }
    else {
        std::vector<uint64_t> distrib0;
        std::vector<uint64_t> distrib1;

        regs[sReg] = 0;
        fastGetDistrib(fc, maskRegs, regs, distrib0, outWidth);

        regs[sReg] = (fc.isBitsliceable && outWidth == 1) ? ~0ULL : 1ULL;
        fastGetDistrib(fc, maskRegs, regs, distrib1, outWidth);

        for (size_t v = 0; v < distrib0.size(); v += 1) {
            if (distrib0[v] != distrib1[v]) {
                return true;
            }
        }
        return false;
    }
}


static void fastGetEffectiveShares(const FastCircuit & fc, const std::vector<int32_t> & publicRegs, const std::vector<Node *> & shareVars, const std::vector<int32_t> & maskRegs, int32_t idx, uint64_t * regs, int32_t outWidth, std::set<Node *> & effectiveShares) {
    if (idx < (int32_t) publicRegs.size()) {
        int32_t r = publicRegs[idx];
        regs[r] = 0;
        fastGetEffectiveShares(fc, publicRegs, shareVars, maskRegs, idx + 1, regs, outWidth, effectiveShares);
        regs[r] = (fc.isBitsliceable && outWidth == 1) ? ~0ULL : 1ULL;
        fastGetEffectiveShares(fc, publicRegs, shareVars, maskRegs, idx + 1, regs, outWidth, effectiveShares);
    }
    else {
        std::vector<int32_t> sharesWithFixedVal;
        for (Node * s : shareVars) {
            if (!effectiveShares.contains(s)) {
                if (!fc.var2reg.contains(s)) {
                    continue;
                }
                int32_t sReg = fc.var2reg.at(s);
                sharesWithFixedVal.clear();
                for (Node * rs : shareVars) {
                    if (rs != s && fc.var2reg.contains(rs)) {
                        sharesWithFixedVal.push_back(fc.var2reg.at(rs));
                    }
                }
                bool sIsEffective = fastShareIsEffective(fc, sharesWithFixedVal, sReg, maskRegs, 0, regs, outWidth);
                if (sIsEffective) {
                    effectiveShares.insert(s);
                }
            }
        }
    }
}


bool isNIWithExev(Node & e, int32_t maxShareOcc) {
    assert(e.width <= 64);

    secPropValidity(e, NI);
    Node & e0 = getBitDecomposition(e);

    std::vector<Node *> allVarsVec;
    std::vector<Node *> exp{&e0};
    getVarsList(exp, allVarsVec);

    std::vector<Node *> shareVarsOrig;
    std::vector<Node *> shareVars;
    std::vector<Node *> publicVars;
    std::vector<Node *> maskVars;
    std::set<Node *> secretVars;

    for (const auto & v : allVarsVec) {
        if (v->symbType == 'A') {
            shareVarsOrig.push_back(v);
        }
        else if (v->symbType == 'P') {
            publicVars.push_back(v);
        }
        else {
            assert(v->symbType == 'M');
            maskVars.push_back(v);
        }
    }
    for (const auto & v : shareVarsOrig) {
        secretVars.insert(v->origSecret);
    }

    for (const auto & v : secretVars) {
        int32_t nbShares = 0;
        for (const auto & sh : shareVarsOrig) {
            if (sh->origSecret == v) {
                nbShares += 1;
            }
        }
        if (nbShares > maxShareOcc) {
            for (const auto & sh : shareVarsOrig) {
                if (sh->origSecret == v) {
                    shareVars.push_back(sh);
                }
            }
        }
        else {
            for (const auto & sh : shareVarsOrig) {
                if (sh->origSecret == v) {
                    publicVars.push_back(sh);
                }
            }
        }
    }

    FastCircuit fc;
    fc.compile(e0);

    std::vector<int32_t> publicRegs;
    for (Node * pv : publicVars) {
        if (fc.var2reg.contains(pv)) {
            publicRegs.push_back(fc.var2reg[pv]);
        }
    }
    std::vector<int32_t> maskRegs;
    for (Node * mv : maskVars) {
        if (fc.var2reg.contains(mv)) {
            maskRegs.push_back(fc.var2reg[mv]);
        }
    }

    std::vector<uint64_t> regs(fc.numRegs, 0);
    std::set<Node *> effectiveShares;
    fastGetEffectiveShares(fc, publicRegs, shareVars, maskRegs, 0, regs.data(), e0.width, effectiveShares);

    for (const auto & s : secretVars) {
        int32_t nbEffectiveShares = 0;
        for (const auto & sh : shareVars) {
            if (sh->origSecret == s and effectiveShares.contains(sh)) {
                nbEffectiveShares += 1;
            }
        }
        if (nbEffectiveShares > maxShareOcc) {
            return false;
        }
    }
    return true;
}


bool isRNIWithExev(Node & e, int32_t diff) {
    assert(e.width <= 64);

    secPropValidity(e, RNI);
    Node & e0 = getBitDecomposition(e);

    std::vector<Node *> allVarsVec;
    std::vector<Node *> exp{&e0};
    getVarsList(exp, allVarsVec);

    std::vector<Node *> shareVarsOrig;
    std::vector<Node *> shareVars;
    std::vector<Node *> publicVars;
    std::vector<Node *> maskVars;
    std::set<Node *> secretVars;

    for (const auto & v : allVarsVec) {
        if (v->symbType == 'A') {
            shareVarsOrig.push_back(v);
        }
        else if (v->symbType == 'P') {
            publicVars.push_back(v);
        }
        else {
            assert(v->symbType == 'M');
            maskVars.push_back(v);
        }
    }
    for (const auto & v : shareVarsOrig) {
        secretVars.insert(v->origSecret);
    }

    for (const auto & v : secretVars) {
        int32_t nbUsedShares = 0;
        int32_t nbTotalShares = v->nbShares;

        for (const auto & sh : shareVarsOrig) {
            if (sh->origSecret == v) {
                nbUsedShares += 1;
            }
        }
        assert(nbUsedShares == (int32_t) e0.shareOcc[v].size());
        if (nbUsedShares + diff >= nbTotalShares) {
            for (const auto & sh : shareVarsOrig) {
                if (sh->origSecret == v) {
                    shareVars.push_back(sh);
                }
            }
        }
        else {
            for (const auto & sh : shareVarsOrig) {
                if (sh->origSecret == v) {
                    publicVars.push_back(sh);
                }
            }
        }
    }

    FastCircuit fc;
    fc.compile(e0);

    std::vector<int32_t> publicRegs;
    for (Node * pv : publicVars) {
        if (fc.var2reg.contains(pv)) {
            publicRegs.push_back(fc.var2reg[pv]);
        }
    }
    std::vector<int32_t> maskRegs;
    for (Node * mv : maskVars) {
        if (fc.var2reg.contains(mv)) {
            maskRegs.push_back(fc.var2reg[mv]);
        }
    }

    std::vector<uint64_t> regs(fc.numRegs, 0);
    std::set<Node *> effectiveShares;
    fastGetEffectiveShares(fc, publicRegs, shareVars, maskRegs, 0, regs.data(), e0.width, effectiveShares);

    for (const auto & s : secretVars) {
        int32_t nbTotalShares = s->nbShares;
        int32_t nbEffectiveShares = 0;
        for (const auto & sh : shareVars) {
            if (sh->origSecret == s and effectiveShares.contains(sh)) {
                nbEffectiveShares += 1;
            }
        }
        if (nbEffectiveShares + diff >= nbTotalShares) {
            return false;
        }
    }
    return true;
}


bool isPINIWithExev(Node & e, int32_t maxShareOcc, std::set<int> & outputIndexes) {
    assert(e.width <= 64);

    secPropValidity(e, PINI);
    Node & e0 = getBitDecomposition(e);

    std::vector<Node *> allVarsVec;
    std::vector<Node *> exp{&e0};
    getVarsList(exp, allVarsVec);

    std::vector<Node *> shareVars;
    std::vector<Node *> publicVars;
    std::vector<Node *> maskVars;
    std::set<Node *> secretVars;

    for (const auto & v : allVarsVec) {
        if (v->symbType == 'A') {
            shareVars.push_back(v);
        }
        else if (v->symbType == 'P') {
            publicVars.push_back(v);
        }
        else {
            assert(v->symbType == 'M');
            maskVars.push_back(v);
        }
    }
    for (const auto & v : shareVars) {
        secretVars.insert(v->origSecret);
    }

    FastCircuit fc;
    fc.compile(e0);

    std::vector<int32_t> publicRegs;
    for (Node * pv : publicVars) {
        if (fc.var2reg.contains(pv)) {
            publicRegs.push_back(fc.var2reg[pv]);
        }
    }
    std::vector<int32_t> maskRegs;
    for (Node * mv : maskVars) {
        if (fc.var2reg.contains(mv)) {
            maskRegs.push_back(fc.var2reg[mv]);
        }
    }

    std::vector<uint64_t> regs(fc.numRegs, 0);
    std::set<Node *> effectiveShares;
    fastGetEffectiveShares(fc, publicRegs, shareVars, maskRegs, 0, regs.data(), e0.width, effectiveShares);

    std::set<int> effectiveShareIndexesNotInOutputIndexes;
    for (const auto & sh : effectiveShares) {
        if (!outputIndexes.contains(sh->shareNum)) {
            effectiveShareIndexesNotInOutputIndexes.insert(sh->shareNum);
        }
    }

    return (int32_t) effectiveShareIndexesNotInOutputIndexes.size() <= maxShareOcc;
}


bool isOPINIWithExev(Node & e, int32_t maxShareOcc, std::set<int> & outputIndexes, std::set<int> & additionalInputIndexes) {
    assert(e.width <= 64);

    secPropValidity(e, OPINI);
    Node & e0 = getBitDecomposition(e);

    std::vector<Node *> allVarsVec;
    std::vector<Node *> exp{&e0};
    getVarsList(exp, allVarsVec);

    std::vector<Node *> shareVars;
    std::vector<Node *> publicVars;
    std::vector<Node *> maskVars;
    std::set<Node *> secretVars;

    for (const auto & v : allVarsVec) {
        if (v->symbType == 'A') {
            shareVars.push_back(v);
        }
        else if (v->symbType == 'P') {
            publicVars.push_back(v);
        }
        else {
            assert(v->symbType == 'M');
            maskVars.push_back(v);
        }
    }
    for (const auto & v : shareVars) {
        secretVars.insert(v->origSecret);
    }

    FastCircuit fc;
    fc.compile(e0);

    std::vector<int32_t> publicRegs;
    for (Node * pv : publicVars) {
        if (fc.var2reg.contains(pv)) {
            publicRegs.push_back(fc.var2reg[pv]);
        }
    }
    std::vector<int32_t> maskRegs;
    for (Node * mv : maskVars) {
        if (fc.var2reg.contains(mv)) {
            maskRegs.push_back(fc.var2reg[mv]);
        }
    }

    std::vector<uint64_t> regs(fc.numRegs, 0);
    std::set<Node *> effectiveShares;
    fastGetEffectiveShares(fc, publicRegs, shareVars, maskRegs, 0, regs.data(), e0.width, effectiveShares);

    for (const auto & sh : effectiveShares) {
        if (!outputIndexes.contains(sh->shareNum)) {
            additionalInputIndexes.insert(sh->shareNum);
        }
    }

    return (int32_t) additionalInputIndexes.size() <= maxShareOcc;
}

} // namespace vmsi
