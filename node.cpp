/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <bit>
#include <cassert>
#include <iomanip>


#include "node.hpp"
#include "config.hpp"
#include "arrayexp.hpp"
#include "SHA256.hpp"


Node::Node() {
    num = nodeNum;
    nodeNum += 1;
    width = 0;
    children = NULL;
    nbShares = 0;
    shareNum = 0;
    symb = NULL;
    symbType = '\0';
    nature = NONE;
    op = NOP;
    hasWordOp = false;
    hasPlus = false;
    wordAnalysisHasFailedOnSubExp = false;
    nlimbs = 0;
    cst = NULL;
    strn = NULL;

    simpEq = NULL;
    simpEqUsbv = NULL;
    origSecret = NULL;
    pseudoShareEq = NULL;

    #if KEEP_SECRET_VAR_OCC
        secretVarOcc = new std::map<Node *, int32_t>();
    #else
        secretVarOcc = new std::set<Node *>();
    #endif
    #if KEEP_PUBLIC_VAR_OCC
        publicVarOcc = new std::map<Node *, int32_t>();
    #else
        publicVarOcc = new std::set<Node *>();
    #endif
    currentlyMasking = new std::map<Node *, Node *>();
    maskingMaskOcc = new std::map<Node *, std::map<Node *, std::map<Node *, std::pair<int32_t, int32_t>> * > * >();
    otherMaskOcc = new std::map<Node *, std::map<Node *, int32_t> * >();
    shareOcc = new std::map<Node *, std::map<Node *, int32_t> * >();
    preservedMask = { NULL, NULL };
    h = NULL;
}


/*
Node::Node(const Node & n) {
    std::cout << "# constructeur par recopie de " << n << std::endl;
    num = n.nodeNum;
    width = n.width;

    if (n.children != NULL) {
        children = new std::vector<Node *>();
        for (const auto & child : *n.children) {
            children->push_back(child);
        }
    }
    else {
        children = NULL;
    }
    nbShares = n.nbShares;
    shareNum = n.shareNum;

    if (n.symb != NULL) {
        symb = new std::string(*n.symb);
    }
    else {
        symb = NULL;
    }
    symbType = n.symbType;
    nature = n.nature;
    op = n.op;
    hasWordOp = n.hasWordOp;
    wordAnalysisHasFailedOnSubExp = n.wordAnalysisHasFailedOnSubExp;
    cst = n.cst;
    if (n.strn != NULL) {
        strn = new std::string(*n.strn);
    }
    else {
        strn = NULL;
    }

    concatExtEq = n.concatExtEq;
    simpEq = n.simpEq;
    simpEqUsbv = n.simpEqUsbv;
    origSecret = n.origSecret;
    pseudoShareEq = n.pseudoShareEq;

    secretVarOcc = new std::map<Node *, int32_t>();
    *secretVarOcc = *n.secretVarOcc;

    publicVarOcc = new std::map<Node *, int32_t>();
    *publicVarOcc = *n.publicVarOcc;

    currentlyMasking = new std::map<Node *, Node *>();
    *currentlyMasking = *n.currentlyMasking;

    otherMaskOcc = new std::map<Node *, std::map<Node *, int32_t> * >();
    for (const auto & [k, v] : *n.otherMaskOcc) {
        std::map<Node *, int32_t> * m = new std::map<Node *, int32_t>();
        for (const auto & [k0, v0] : *v) {
            m->emplace(k0, v0);
        }
        otherMaskOcc->emplace(k, m);
    }

    shareOcc = new std::map<Node *, std::map<Node *, int32_t> * >();
    for (const auto & [k, v] : *n.shareOcc) {
        std::map<Node *, int32_t> * m = new std::map<Node *, int32_t>();
        for (const auto & [k0, v0] : *v) {
            m->emplace(k0, v0);
        }
        shareOcc->emplace(k, m);
    }

    maskingMaskOcc = new std::map<Node *, std::map<Node *, std::map<Node *, std::pair<int32_t, int32_t>> * > * >();
    for (const auto & [k, v] : *n.maskingMaskOcc) {
        std::map<Node *, std::map<Node *, std::pair<int32_t, int32_t>> * > * m = new std::map<Node *, std::map<Node *, std::pair<int32_t, int32_t>> * >();
        for (const auto & [k0, v0] : *v) {
            std::map<Node *, std::pair<int32_t, int32_t>> * m0 = new std::map<Node *, std::pair<int32_t, int32_t>>();
            for (const auto & [k1, v1] : *v0) {
                m0->emplace(k1, v1);
            }
            m->emplace(k0, m0);
        }
        maskingMaskOcc->emplace(k, m);
    }


    // Potentially incorrect as in the printMask function, we test if one of the preserved mask is "this"
    preservedMask = n.preservedMask;
    h = new uint64_t[4];
    for (int32_t i = 0; i < 4; i += 1) {
        h[i] = n.h[i];
    }

}
*/

Node::~Node() {
    //std::cout << "# Deleting node: " << *this << std::endl;
    if (symb != NULL) {
        std::cout << "# deleting symbol " << *symb << std::endl;
    }
    delete secretVarOcc;
    delete publicVarOcc;
    delete currentlyMasking;
    for (const auto & [m0, v0] : *maskingMaskOcc) {
        for (const auto & [m1, v1] : *maskingMaskOcc->at(m0)) {
            delete maskingMaskOcc->at(m0)->at(m1);
        }
        delete maskingMaskOcc->at(m0);
    }
    delete maskingMaskOcc;
    for (const auto & [m0, v0] : *otherMaskOcc) {
        delete otherMaskOcc->at(m0);
    }
    delete otherMaskOcc;
    for (const auto & [m0, v0] : *shareOcc) {
        delete shareOcc->at(m0);
    }
    delete shareOcc;
    delete [] this->h;
    if (symb != NULL) {
        delete symb;
    }
    if (strn != NULL) {
        delete strn;
    }
    if (children != NULL) {
        delete children;
    }
    if (cst != NULL) {
        delete cst;
    }
}


void Node::setModeTemporaryNodes() {
    Node::modeTempNode = true;
}

void Node::setModePermanentNodes() {
    Node::modeTempNode = false;
    #if MEMORY_STRATEGY == DELETE_NODES
        for (const auto & node : toDelete) {
            delete node;
        }
        toDelete.clear();
    #endif
}


Node & Node::SymbNode(const std::string & symb, char symbType, int32_t width, int32_t nbShares, int32_t shareNum, Node * origSecret, Node * pseudoShareEq) {
    assert(symbType != 'A' or nbShares != 0);
    assert(symbType != 'A' or origSecret != NULL);
    assert(symbType != 'A' or pseudoShareEq != NULL);
    Node * n = new Node();
    n->nature = SYMB;
    n->symb = new std::string(symb);
    n->symbType = symbType;
    n->width = width;
    for (int32_t i = 0; i < width; i += 1) {
        n->extractBit.push_back(NULL);
    }
    n->nbShares = nbShares;
    n->shareNum = shareNum;
    n->origSecret = origSecret;
    n->pseudoShareEq = pseudoShareEq;
    n->setVarsOccurrences();
    n->simpEq = n;
    SHA256 sha;
    sha.update(symb);
    n->h = sha.digest();
    return *n;
}


Node & Node::ConstNode(uint64_t cst, int32_t width, bool extendMSB) {
    if (width <= 64) {
        Node * n = new Node();
        n->nature = CONST;
        n->nlimbs = 1;
        n->cst = new uint64_t[1];
        n->cst[0] = cst;
        n->width = width;
        n->simpEq = n;
        SHA256 sha;
        sha.update(std::to_string(width) + std::to_string(cst));
        n->h = sha.digest();
        return *n;
    }
    else {
        int32_t nlimbs = width / 64;
        if (nlimbs * 64 != width) {
            nlimbs += 1;
        }
        std::string s = std::to_string(width);
        s += std::to_string(cst);
        for (int32_t i = 1; i < nlimbs; i += 1) {
            s += std::to_string(0);
        }

        SHA256 sha;
        sha.update(s);
        uint64_t * h = sha.digest();
        std::tuple<uint64_t, uint64_t, uint64_t, uint64_t> t {h[0], h[1], h[2], h[3]};
        if (bigCst2node.contains(t)) {
            delete [] h;
            return *bigCst2node[t];
        }
        Node * n = new Node();
        bigCst2node[t] = n;

        n->nature = CONST;
        n->nlimbs = nlimbs;
        n->cst = new uint64_t[nlimbs];
        n->cst[0] = cst;
        if (extendMSB && (cst >> 63 == 1)) {
            for (int32_t i = 1; i < nlimbs; i += 1) {
                n->cst[i] = 0xffffffffffffffffULL;
            }
            if (width % 64 != 0) {
                n->cst[nlimbs - 1] >>= (64 - (width % 64));
            }
        }
        else {
            for (int32_t i = 1; i < nlimbs; i += 1) {
                n->cst[i] = 0;
            }
        }
        n->width = width;
        n->simpEq = n;
        n->h = h;
        return *n;
    }
}


Node & Node::ConstNode(uint64_t * cst, int32_t nlimbs, int32_t width) {
    assert(nlimbs > 1);
    std::string s = std::to_string(width);
    for (int32_t i = 0; i < nlimbs; i += 1) {
        s += std::to_string(cst[i]);
    }

    SHA256 sha;
    sha.update(s);
    uint64_t * h = sha.digest();
    std::tuple<uint64_t, uint64_t, uint64_t, uint64_t> t {h[0], h[1], h[2], h[3]};
    if (bigCst2node.contains(t)) {
        delete [] h;
        return *bigCst2node[t];
    }
    Node * n = new Node();
    bigCst2node[t] = n;

    n->nature = CONST;
    n->nlimbs = nlimbs;
    n->cst = new uint64_t[nlimbs];
    for (int32_t i = 0; i < nlimbs; i += 1) {
        n->cst[i] = cst[i];
    }
    n->width = width;
    n->simpEq = n;
    n->h = h;
    return *n;
}


Node & Node::ConstNodeAuto(uint64_t cst) {
    return Const(cst, bit_width(cst));
}


Node & Node::StrNode(const std::string & s) {
    Node * n = new Node();
    n->nature = STR;
    n->strn = new std::string(s);
    n->simpEq = n;
    SHA256 sha;
    sha.update(s);
    n->h = sha.digest();
    return *n;
}


Node & Node::OpNode(NodeOp op, const std::vector<Node *> & children) {
    std::vector<Node *> * orderedChildren = new std::vector<Node *>();
    for (const auto & child : children) {
        orderedChildren->push_back(child);
    }


    if (associativeOps.contains(op)) {
        std::sort(orderedChildren->begin(), orderedChildren->end(), [](Node * a, Node * b) {
                int32_t i = 0;
                while (i < 4 && a->h[i] == b->h[i]) {
                    i += 1;
                }
                return i < 4 && a->h[i] > b->h[i];
                });
    }
    std::string s = std::to_string(op);
    for (const auto & child : *orderedChildren) {
        s += SHA256::toString(child->h);
    }
    SHA256 sha;
    sha.update(s);
    uint64_t * h = sha.digest();
    Node * n;
    #if MEMORY_STRATEGY == KEEP_NODES
        std::tuple<uint64_t, uint64_t, uint64_t, uint64_t> t {h[0], h[1], h[2], h[3]};
        if (cache.contains(t)) {
            delete [] h;
            delete orderedChildren;
            return *cache[t];
        }
        n = new Node();
        cache[t] = n;
    #else
        if (!modeTempNode) {
            std::tuple<uint64_t, uint64_t, uint64_t, uint64_t> t {h[0], h[1], h[2], h[3]};
            if (cache.contains(t)) {
                delete [] h;
                delete orderedChildren;
                return *cache[t];
            }
            n = new Node();
            cache[t] = n;
        }
        else {
            n = new Node();
        }
    #endif

    n->h = h;
    n->children = orderedChildren;
    n->nature = OP;
    n->op = op;
    n->hasWordOp = (op == IMUL or op == IPOW or op == GMUL or op == GPOW or op == GLOG or op == GEXP);
    if (!n->hasWordOp) {
        for (const auto & c: children) {
            if (c->hasWordOp) {
                n->hasWordOp = true;
                break;
            }
        }
    }
    n->hasPlus = (op == PLUS);
    if (!n->hasPlus) {
        for (const auto & c: children) {
            if (c->hasPlus) {
                n->hasPlus = true;
                break;
            }
        }
    }

    for (const auto & c: children) {
        if (c->wordAnalysisHasFailedOnSubExp) {
            n->wordAnalysisHasFailedOnSubExp = true;
            break;
        }
    }

    if (op == PLUS or op == MINUS or op == UMINUS or op == BAND or op == BOR or op == BXOR or op == BNOT or op == LSHL or op == ASHR or op == LSHR or op == SLSHL or op == SASHR or op == SLSHR or op == IMUL or op == IPOW or op == GMUL or op == GPOW or op == GLOG or op == GEXP) {
        n->width = children[0]->width;
    }
    else if (op == ZEXT or op == SEXT) {
        assert(children[0]->nlimbs == 1);
        n->width = children[0]->cst[0] + children[1]->width;
    }
    else if (op == CONCAT) {
        int32_t width = 0;
        for (const auto & child: children) {
            width += child->width;
        }
        n->width = width;
    }
    else if (op == EXTRACT) {
        assert(children[0]->nlimbs == 1 && children[1]->nlimbs == 1);
        n->width = children[0]->cst[0] - children[1]->cst[0] + 1;
    }
    else if (op == ARRAY) {
        n->width = ArrayExp::allArrays[*children[0]->strn]->outWidth;
    }
    else {
        std::cerr << "*** Error op = " << op << std::endl;
        exit(EXIT_FAILURE);
    }

    for (int32_t i = 0; i < n->width; i += 1) {
        n->extractBit.push_back(NULL);
    }

    n->setVarsOccurrences();

    // Examples to have in mind
    //
    //          +          ^         +       +      +     +
    //         /|\        /|\       / \     / \    / \   / \ .
    //        / | \      e n ~      \ /     \ /    \ /   ~ ~
    //      m1  ^  m0        |       ~       m      ~    | |
    //         / \           ^       |              |    m m
    //        m0  k         / \      ^              m
    //                     k   ~    / \ .
    //                         |   k   m
    //                         m


    // Occurrences in currentlyMasking are also present in maskingMaskOcc
    // Mask in preservedMask is also present in otherMaskOcc

    // "Masking" mask occurrences
    for (const auto & child: children) {
        for (const auto & [m, val0] : *child->maskingMaskOcc) {
            if (not n->maskingMaskOcc->contains(m)) {
                n->maskingMaskOcc->emplace(m, new std::map<Node *, std::map<Node *, std::pair<int32_t, int32_t>> * >());
            }
            for (const auto & [ctrBase, val1] : *child->maskingMaskOcc->at(m)) {
                if (not n->maskingMaskOcc->at(m)->contains(ctrBase)) {
                    n->maskingMaskOcc->at(m)->emplace(ctrBase, new std::map<Node *, std::pair<int32_t, int32_t>>());
                }
                for (const auto & [ctr, val2] : *child->maskingMaskOcc->at(m)->at(ctrBase)) {
                    if (not n->maskingMaskOcc->at(m)->at(ctrBase)->contains(ctr)) {
                        //std::pair<int32_t, int32_t> entry = child.maskingMaskOcc[m][ctrBase][ctr];
                        // entry[0] : count (number of occurrences)
                        // entry[1] : height of ctr starting from ctrBase (0 for ctrBase)
                        n->maskingMaskOcc->at(m)->at(ctrBase)->emplace(ctr, child->maskingMaskOcc->at(m)->at(ctrBase)->at(ctr));
                    }
                    else {
                        n->maskingMaskOcc->at(m)->at(ctrBase)->at(ctr).first += child->maskingMaskOcc->at(m)->at(ctrBase)->at(ctr).first;
                    }
                }
            }
        }
    }


    // For other mask occurrences, we do the union of all the children
    for (const auto & child: children) {
        for (const auto & [m, val0] : *child->otherMaskOcc) {
            if (not n->otherMaskOcc->contains(m)) {
                n->otherMaskOcc->emplace(m, new std::map<Node *, int32_t>());
            }
            if (child->otherMaskOcc->at(m) == NULL) {
                if (n->otherMaskOcc->at(m)->contains(n)) {
                    n->otherMaskOcc->at(m)->at(n) += 1;
                }
                else {
                    n->otherMaskOcc->at(m)->emplace(n, 1);
                }
            }
            else {
                for (const auto & [p, val1] : *child->otherMaskOcc->at(m)) {
                    if (n->otherMaskOcc->at(m)->contains(p)) {
                        n->otherMaskOcc->at(m)->at(p) += child->otherMaskOcc->at(m)->at(p);
                    }
                    else {
                        n->otherMaskOcc->at(m)->emplace(p, child->otherMaskOcc->at(m)->at(p));
                    }
                }
            }
        }
    }


    // currentlyMasking: Masks masking current node, value is ctrBase
    // preservedMask: bijection of a mask, can mask if a maskingNode is encountered. tuple if not None: (mask, parent)
    if (maskingOps.contains(op)) {
        // Conditions for a mask to become a "currently masking" mask in a maskingOp node:
        // - it must be a "currently masking" mask or a preserved mask of one of the children
        // - it must not have other occurrences in any other child (masking or other occ)
        for (int32_t i = 0; i < (int32_t) children.size(); i += 1) {
            Node * child = children[i];
            // FIXME: can it be done in one pass? (without the 2nd for loop on children)
            for (const auto & [m, val] : *child->currentlyMasking) {
                bool maskIsMasking = true;
                for (int32_t j = 0; j < (int32_t) children.size(); j += 1) {
                    if (i == j) {
                        continue;
                    }
                    Node * other = children[j];
                    if (other->maskingMaskOcc->contains(m) or other->otherMaskOcc->contains(m)) {
                        maskIsMasking = false;
                        break;
                    }
                }
                if (maskIsMasking) {
                    n->currentlyMasking->emplace(m, child->currentlyMasking->at(m));
                    // Adding mask to masking mask occ
                    int32_t height = child->maskingMaskOcc->at(m)->at(child->currentlyMasking->at(m))->at(child).second;
                    n->maskingMaskOcc->at(m)->at(child->currentlyMasking->at(m))->emplace(n, std::make_pair(1, height + 1));
                }
            }

            if (child->preservedMask.first != NULL) {
                Node * m = child->preservedMask.first;
                Node * parent = child->preservedMask.second;
                bool maskIsMasking = true;
                for (int32_t j = 0; j < (int32_t) children.size(); j += 1) {
                    if (i == j) {
                        continue;
                    }
                    Node * other = children[j];
                    if (other->maskingMaskOcc->contains(m) or other->otherMaskOcc->contains(m)) {
                        maskIsMasking = false;
                        break;
                    }
                }
                if (maskIsMasking) {
                    n->currentlyMasking->insert_or_assign(m, n);

                    // Removing mask occurrence from otherOccurrences
                    if (parent == NULL) {
                        parent = n;
                    }
                    n->otherMaskOcc->at(m)->at(parent) -= 1;
                    if (n->otherMaskOcc->at(m)->at(parent) == 0) {
                        n->otherMaskOcc->at(m)->erase(parent);
                        if (n->otherMaskOcc->at(m)->size() == 0) {
                            delete n->otherMaskOcc->at(m); // delete map
                            n->otherMaskOcc->erase(m);
                        }
                    }

                    // Adding mask to masking mask occurrences
                    if (not n->maskingMaskOcc->contains(m)) {
                        n->maskingMaskOcc->emplace(m, new std::map<Node *, std::map<Node *, std::pair<int32_t, int32_t>> * >());
                    }
                    n->maskingMaskOcc->at(m)->emplace(n, new std::map<Node *, std::pair<int32_t, int32_t>>());
                    n->maskingMaskOcc->at(m)->at(n)->emplace(n, std::make_pair(1, 0));
                }
            }
        }
    }

    else if (preserveMaskingOps.contains(op)) {
        Node * child;
        if (op == ARRAY) {
            child = children[1];
        }
        else {
            child = children[0];
        }

        if (child->preservedMask.first != NULL) {
            if (child->preservedMask.second != NULL) {
                n->preservedMask = child->preservedMask;
            }
            else {
                n->preservedMask = {child->preservedMask.first, n};
            }
        }

        *n->currentlyMasking = *child->currentlyMasking;
        // Creating new masking occurrences for masks masking current node
        for (const auto & [m, val] : *n->currentlyMasking) {
            Node * ctrBase = n->currentlyMasking->at(m);
            int32_t height = child->maskingMaskOcc->at(m)->at(ctrBase)->at(child).second;
            n->maskingMaskOcc->at(m)->at(ctrBase)->emplace(n, std::make_pair(1, height + 1));
        }
    }

    // if node is neither a masking node nor a preserving node, currently masking masks and preserved masks are empty

    if (modeTempNode) {
        #if MEMORY_STRATEGY == DELETE_NODES
            toDelete.insert(n);
        #else
            opNodes.insert(n);
        #endif
    }
    else {
        opNodes.insert(n);
    }

    return *n;
}


const char * Node::op2strOp(NodeOp op) {
    switch(op) {
        case NOP:
            return "NOP";
        case BXOR:
            return "^";
        case BAND:
            return "&";
        case BOR:
            return "|";
        case BNOT:
            return "~";
        case LSHR:
            return "LShR";
        case LSHL:
            return "<<";
        case ASHR:
            return ">>";
        case SLSHR:
            return "SLShR";
        case SLSHL:
            return "s<<";
        case SASHR:
            return "s>>";
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case UMINUS:
            return "-";
        case IMUL:
            return "*";
        case IPOW:
            return "**";
        case GMUL:
            return "M";
        case GPOW:
            return "P";
        case GLOG:
            return "L";
        case GEXP:
            return "X";
        case ZEXT:
            return "ZE";
        case SEXT:
            return "SE";
        case CONCAT:
            return "C";
        case EXTRACT:
            return "E";
        case ARRAY:
            return "A";
        case INPUT:
            return "I";
        default:
            std::cout << "*** Error: op " << op << " not defined" << std::endl;
            return "Error";
            //assert(false);
    }
}


void Node::printVarOcc() {
    std::cout << "Secrets: {";
    #if KEEP_SECRET_VAR_OCC
        for (const auto & [m, val] : *secretVarOcc) {
            std::cout << m->symb << ": " << secretVarOcc->at(m) << "; ";
        }
    #else
        for (const auto & m : *secretVarOcc) {
            std::cout << m->symb << "; ";
        }
    #endif
    std::cout << "}" << std::endl;
    std::cout << "Public: {";
    #if KEEP_PUBLIC_VAR_OCC
        for (const auto & [m, val] : *publicVarOcc) {
            std::cout << m->symb << ": " << publicVarOcc->at(m) << "; ";
        }
    #else
        for (const auto & m : *publicVarOcc) {
            std::cout << m->symb << "; ";
        }
    #endif
    std::cout << "}" << std::endl;
}


void Node::printMaskOcc() {
    if (preservedMask.first != NULL) {
        std::string ch1;
        if (preservedMask.second == NULL) {
            ch1 = "NULL";
        }
        else if (preservedMask.second == this) {
            ch1 = "this";
        }
        else {
            ch1 = *preservedMask.second->symb;
        }
        std::cout << "# Preserved Mask: (" << *preservedMask.first << ", " << ch1 << ")" << std::endl;
    }
    else {
        std::cout << "# Preserved Mask: NULL" << std::endl;
    }

    std::cout << "# Currently masking:" << std::endl;
    for (const auto & [m, val0] : *currentlyMasking) {
        std::cout << "#    Mask " << *m << std::endl;
        std::cout << "#        CTR: " << *currentlyMasking->at(m) << std::endl;
    }
    std::cout << "# Masking mask occurrences:" << std::endl;
    for (const auto & [m, val0] : *maskingMaskOcc) {
        std::cout << "#    Mask " << *m << std::endl;
        for (const auto & [ctrBase, val1] : *maskingMaskOcc->at(m)) {
            std::cout << "#        ctrBase " << *ctrBase << std::endl;
            for (const auto & [ctr, val2] : *maskingMaskOcc->at(m)->at(ctrBase)) {
                std::cout << "#            ctr " << *ctr << std::endl;
                std::cout << "#                count:  " << maskingMaskOcc->at(m)->at(ctrBase)->at(ctr).first << std::endl;
                std::cout << "#                height: " << maskingMaskOcc->at(m)->at(ctrBase)->at(ctr).second << std::endl;
            }
        }
    }
    std::cout << "# Other mask occurrences:" << std::endl;
    for (const auto & [m, val0] : *otherMaskOcc) {
        std::cout << "#    Mask " << *m << std::endl;
        for (const auto & [p, val1] : *otherMaskOcc->at(m)) {
            std::cout << "#        Parent: " << *p << std::endl;
            std::cout << "#            count: " << otherMaskOcc->at(m)->at(p) << std::endl;
        }
    }
}



void Node::setVarsOccurrences() {
    if (nature == SYMB) {
        if (symbType == 'M') {
            preservedMask = { this, NULL }; // (mask, parent)
            otherMaskOcc->insert({this, NULL});
        }
        #if KEEP_PUBLIC_VAR_OCC
        else if (symbType == 'P') {
            publicVarOcc->emplace(this, 1);
        }
        #else
        else if (symbType == 'P') {
            publicVarOcc->insert(this);
        }
        #endif
        #if KEEP_SECRET_VAR_OCC
        else if (symbType == 'S') {
            secretVarOcc->emplace(this, 1);
        }
        #else
        else if (symbType == 'S') {
            secretVarOcc->insert(this);
        }
        #endif
        else if (symbType == 'A') {
            shareOcc->emplace(origSecret, new std::map<Node *, int32_t>());
            shareOcc->at(origSecret)->emplace(this, 1);
        }
        return;
    }
    for (const auto & child: *children) {
        #if KEEP_PUBLIC_VAR_OCC
            for (const auto & [p, val] : *child->publicVarOcc) {
                if (publicVarOcc->contains(p)) {
                    publicVarOcc->at(p) += child->publicVarOcc->at(p);
                }
                else {
                    publicVarOcc->emplace(p, child->publicVarOcc->at(p));
                }
            }
        #else
            for (const auto & p : *child->publicVarOcc) {
                publicVarOcc->insert(p);
            }
        #endif
        #if KEEP_SECRET_VAR_OCC
            for (const auto & [k, val] : *child->secretVarOcc) {
                if (secretVarOcc->contains(k)) {
                    secretVarOcc->at(k) += child->secretVarOcc->at(k);
                }
                else {
                    secretVarOcc->emplace(k, child->secretVarOcc->at(k));
                }
            }
        #else
            for (const auto & k : *child->secretVarOcc) {
                secretVarOcc->insert(k);
            }
        #endif
        for (const auto & [s, val0] : *child->shareOcc) {
            if (not shareOcc->contains(s)) {
                shareOcc->emplace(s, new std::map<Node *, int32_t>());
            }
            for (const auto & [a, val1] : *child->shareOcc->at(s)) {
                if (shareOcc->at(s)->contains(a)) {
                    shareOcc->at(s)->at(a) += child->shareOcc->at(s)->at(a);
                }
                else {
                    shareOcc->at(s)->emplace(a, child->shareOcc->at(s)->at(a));
                }
            }
        }
    }
}


void Node::fillReachableNodes(Node * n, std::set<Node *> & rn) {
    if (not rn.contains(n)) {
        rn.insert(n);
    }
    if (n->children != NULL) {
        for (const auto & child: *n->children) {
            fillReachableNodes(child, rn);
        }
    }
}


void Node::dumpNodes(const char * filename, std::set<Node *> & nodes) {
    std::string content = "digraph g {\n";
    for (const auto & n: nodes) {
        std::string s;
        if (n->nature == SYMB) {
            s = std::string("Symbol: ") + *n->symb + " [" + n->symbType + "]";
        }
        else if (n->nature == OP) {
            if (n->op == BOR) {
                s = std::string("Op: \\| (num ") + std::to_string(n->num) + ")";
            }
            else if (n->op == LSHL) {
                s = std::string("Op: \\<\\< (num ") + std::to_string(n->num) + ")";
            }
            else if (n->op == LSHR) {
                s = std::string("Op: LShR (num ") + std::to_string(n->num) + ")";
            }
            else if (n->op == ASHR) {
                s = std::string("Op: \\>\\> (num ") + std::to_string(n->num) + ")";
            }
            else if (n->op == SLSHL) {
                s = std::string("Op: s\\<\\< (num ") + std::to_string(n->num) + ")";
            }
            else if (n->op == SLSHR) {
                s = std::string("Op: SLShR (num ") + std::to_string(n->num) + ")";
            }
            else if (n->op == SASHR) {
                s = std::string("Op: s\\>\\> (num ") + std::to_string(n->num) + ")";
            }

            else {
                s = std::string("Op: ") + Node::op2strOp(n->op) + " (num " + std::to_string(n->num) + ")";
            }
        }
        else if (n->nature == CONST) {
            s = std::string("Const: ") + n->printCst();
        }
        else if (n->nature == STR) {
            s = std::string("Str: ") + *n->strn;
        }
        else {
            s = "";
        }
        content += std::string("   N") + std::to_string(n->num) + " [shape=record, label=\"{{" + s + "}}\"];\n";
        if (n->children != NULL) {
            for (const auto & a: *n->children) {
                content += "   edge[tailclip=true];\n";
                content += std::string("   N") + std::to_string(n->num) + " -> N" + std::to_string(a->num) + "\n";
            }
        }
    }
    content += "}\n";
    std::ofstream out(filename);
    out << content;
    out.close();
}


Node & Node::makeBitwiseNode(NodeOp op, Node * child0, Node * child1) {
    int32_t width = child0->width;
    assert(child1 == NULL || child0->width == child1->width);

    if (propagateCstOnBuild() && child0->nature == CONST && (child1 == NULL || child1->nature == CONST)) {
        int32_t nlimbs = child0->nlimbs;
        uint64_t res[nlimbs];
        if (op == BXOR) {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                res[i] = child0->cst[i] ^ child1->cst[i];
            }
        }
        else if (op == BAND) {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                res[i] = child0->cst[i] & child1->cst[i];
            }
        }
        else if (op == BOR) {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                res[i] = child0->cst[i] | child1->cst[i];
            }
        }
        else if (op == BNOT) {
            for (int32_t i = 0; i < nlimbs - 1; i += 1) {
                res[i] = ~child0->cst[i];
            }
            if (width % 64 == 0) {
                res[nlimbs - 1] = ~child0->cst[nlimbs - 1];
            }
            else {
                res[nlimbs - 1] = ((1ULL << (width % 64)) - 1) ^ child0->cst[nlimbs - 1];
            }
        }
        else if (op == PLUS) {
            int32_t carry = 0;
            for (int32_t i = 0; i < nlimbs - 1; i += 1) {
                res[i] = child0->cst[i] + child1->cst[i] + carry;
                carry = res[i] < child0->cst[i] ? 1 : 0;
            }
            if (width % 64 == 0) {
                res[nlimbs - 1] = child0->cst[nlimbs - 1] + child1->cst[nlimbs - 1] + carry;
            }
            else {
                res[nlimbs - 1] = (child0->cst[nlimbs - 1] + child1->cst[nlimbs - 1] + carry) % (1ULL << (width % 64));
            }
        }
        else {
            assert(false);
        }
        return Const(res, nlimbs, width);
    }
    if (child1 != NULL) {
        return Node::OpNode(op, {child0, child1});
    }
    else {
        return Node::OpNode(op, {child0});
    }

}



void Node::dump(const char * filename) {
    std::set<Node *> reachableNodes;
    fillReachableNodes(this, reachableNodes);
    dumpNodes(filename, reachableNodes);
}

Node & Node::operator&(Node & other) {
    return makeBitwiseNode(BAND, this, &other);
}

Node & Node::operator|(Node & other) {
    return makeBitwiseNode(BOR, this, &other);
}

Node & Node::operator^(Node & other) {
    return makeBitwiseNode(BXOR, this, &other);
}

Node & Node::operator~() {
    return makeBitwiseNode(BNOT, this, NULL);
}

Node & Node::operator+(Node & other) {
    return makeBitwiseNode(PLUS, this, &other);
}

Node & Node::operator-(Node & other) {
    if (propagateCstOnBuild() && nature == CONST && other.nature == CONST) {
        uint64_t res[nlimbs];
        int32_t carry = 1;
        for (int32_t i = 0; i < nlimbs; i += 1) {
            res[i] = cst[i] + ~other.cst[i] + carry;
            carry = res[i] < ~other.cst[i] ? 1 : 0;
        }
        if (width % 64 != 0) {
            res[nlimbs - 1] = res[nlimbs - 1] % (1ULL << (width % 64));
        }
        return Const(res, nlimbs, width);
    }
    return *this + (-other);
}


Node & Node::operator-() {
    if (propagateCstOnBuild() && nature == CONST) {
        uint64_t res[nlimbs];
        int32_t carry = 1;
        for (int32_t i = 0; i < nlimbs; i += 1) {
            res[i] = ~cst[i] + carry;
            carry = res[i] < ~cst[i] ? 1 : 0;
        }
        if (width % 64 != 0) {
            res[nlimbs - 1] = res[nlimbs - 1] % (1ULL << (width % 64));
        }
        return Const(res, nlimbs, width);
    }
    std::vector<Node *> args(1, this);
    return Node::OpNode(UMINUS, args);
}


// We guaranty that the node used for the shift value is always identical for a given amount
// In particular, it does not depend on the width of this value (which can be non-existant)
Node & Node::operator<<(Node & other) {
    if (other.nature != CONST) {
        return Node::OpNode(SLSHL, {this, &other});
    }
    assert(other.nlimbs == 1);
    return *this << other.cst[0];
}


Node & Node::operator<<(int32_t shval) {
    if (shval >= width) {
        std::cout << "*** Warning: shift value (" << shval << ") >= bit width of expression (" << width << ")" << std::endl;
    }

    if (propagateCstOnBuild() && nature == CONST) {
        uint64_t res[nlimbs];
        int32_t limb_shift = shval / 64;
        int32_t limb_rem = shval % 64;
        if (limb_rem == 0) {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                if (i - limb_shift < 0) {
                    res[i] = 0ULL;
                }
                else {
                    res[i] = cst[i - limb_shift];
                }
            }
        }
        else {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                if (i - limb_shift < 0) {
                    res[i] = 0ULL;
                }
                else if (i - limb_shift == 0) {
                    res[i] = cst[0] << limb_rem;
                }
                else {
                    res[i] = (cst[i - limb_shift] << limb_rem) | (cst[i - limb_shift - 1] >> (64 - limb_rem));
                }
            }
        }
        if (width % 64 != 0) {
            res[nlimbs - 1] = res[nlimbs - 1] % (1ULL << (width % 64));
        }

        return Const(res, nlimbs, width);
    }

    Node & sh = Const((uint64_t) shval, bit_width((uint64_t) shval));
    return Node::OpNode(LSHL, {this, &sh});
}


Node & Node::operator>>(Node & other) {
    if (other.nature != CONST) {
        return Node::OpNode(SASHR, {this, &other});
    }
    assert(other.nlimbs == 1);
    return *this >> other.cst[0];
}


Node & Node::operator>>(int32_t shval) {
    // Arith Shift Right
    if (shval >= width) {
        std::cout << "*** Warning: shift value (" << shval << ") >= bit width of expression (" << width << ")" << std::endl;
    }

    if (propagateCstOnBuild() && nature == CONST) {
        uint64_t res[nlimbs];
        int32_t limb_shift = shval / 64;
        int32_t limb_rem = shval % 64;
        int32_t mslWidth = width % 64; // Width of Most Significant Limb
        if (limb_rem == 0) {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                if (i + limb_shift >= nlimbs) {
                    uint64_t w = (int64_t) (cst[nlimbs - 1] << (64 - limb_rem)) >> 63;
                    res[i] = w;
                }
                else {
                    res[i] = cst[i + limb_shift];
                }
            }
        }
        else {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                if (i + limb_shift >= nlimbs) {
                    uint64_t w;
                    if (mslWidth == 0) {
                        w = ((int64_t) cst[nlimbs - 1]) >> 63;
                    }
                    else {
                        w = ((int64_t) (cst[nlimbs - 1] << (64 - mslWidth))) >> 63;
                    }
                    res[i] = w;
                }
                else if (i + limb_shift == nlimbs - 1) {
                    res[i] = cst[nlimbs - 1] >> limb_rem | (int64_t) ((cst[nlimbs - 1] >> (mslWidth - 1)) << 63) >> limb_rem;
                }
                else {
                    res[i] = cst[i + limb_shift] >> limb_rem | cst[i + limb_shift + 1] << (64 - limb_rem);
                }
            }
        }
        if (width % 64 != 0) {
            res[nlimbs - 1] = res[nlimbs - 1] & ((1ULL << mslWidth) - 1);
        }

        return Const(res, nlimbs, width);
    }

    Node & sh = Const((uint64_t) shval, bit_width((uint64_t) shval));
    return Node::OpNode(ASHR, {this, &sh});
}


Node & Node::operator*(Node & other) {
    assert(width == other.width);
    if (propagateCstOnBuild() && nature == CONST && other.nature == CONST) {
        return imul(*this, other);
    }
    return Node::OpNode(IMUL, {this, &other});
}


std::string Node::toString() const {
    return expPrint(false, false);
}


std::string Node::verbatimPrint() const {
    return expPrint(false, true);
}


std::string Node::printCst() const {
    if (nlimbs == 0) {
        return "";
    }

    std::string s = std::string("0x");
    {
        int32_t limbWidth;
        if (width % 64 == 0) {
            limbWidth = 16;
        }
        else {
            limbWidth = (width - (width / 64) * 64);
            if (limbWidth % 4 == 0) {
                limbWidth = limbWidth / 4;
            }
            else {
                limbWidth = limbWidth / 4 + 1;
            }
        }
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(limbWidth) << std::hex << cst[nlimbs - 1];
        s += stream.str();
    }

    for (int32_t i = nlimbs - 2; i >= 0; i -= 1) {
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(16) << std::hex << cst[i];
        s += stream.str();
    }
    return s;
}


std::string Node::expPrint(bool parNeeded, bool verbatim) const {
    if (nature == SYMB) {
        return *symb;
    }
    else if (nature == CONST) {

        if (verbatim) {
            return std::string("Const(") + printCst() + ", " + std::to_string(width) + ")";
        }
        else {
            return printCst();
        }
    }
    else if (nature == STR) {
        return *strn;
    }

    // OpNode
    assert(nature == OP);
    if (op == BNOT) {
        return std::string("~") + children->at(0)->expPrint(true, verbatim);
    }
    else if (op == UMINUS) {
        return std::string("-") + children->at(0)->expPrint(true, verbatim);
    }
    else if (op == ZEXT) {
        return std::string("ZeroExt(") + children->at(0)->printCst() + ", " + children->at(1)->expPrint(false, verbatim) + ")";
    }
    else if (op == SEXT) {
        return std::string("SignExt(") + children->at(0)->printCst() + ", " + children->at(1)->expPrint(false, verbatim) + ")";
    }
    else if (op == CONCAT) {
        std::string res = "Concat(";
        for (int32_t i = children->size() - 1; i > 0; i -= 1) {
            res += children->at(i)->expPrint(false, verbatim) + ", ";
        }
        res += children->at(0)->expPrint(false, verbatim) + ")";
        return res;
    }
    else if (op == EXTRACT) {
        return std::string("Extract(") + children->at(0)->printCst() + ", " + children->at(1)->printCst() + ", " + children->at(2)->expPrint(false, verbatim) + ")";
    }
    else if (op == ARRAY) {
        return *children->at(0)->strn + "[" + children->at(1)->expPrint(false, verbatim) + "]";
    }
    else if (op == LSHR) {
        return std::string("LShR(") + children->at(0)->expPrint(false, verbatim) + ", " + children->at(1)->printCst() + ")";
    }
    else if (op == ASHR) {
        std::string res = children->at(0)->expPrint(false, verbatim) + " >> " + children->at(1)->printCst();
        if (parNeeded) {
            res = std::string("(") + res + ")";
        }
        return res;
    }
    else if (op == LSHL) {
        std::string res = children->at(0)->expPrint(false, verbatim) + " << " + children->at(1)->printCst();
        if (parNeeded) {
            res = std::string("(") + res + ")";
        }
        return res;
    }
    else if (op == SLSHR) {
        return std::string("SLShR(") + children->at(0)->expPrint(false, verbatim) + ", " + children->at(1)->expPrint(false, verbatim) + ")";
    }
    else if (op == SASHR) {
        std::string res = children->at(0)->expPrint(false, verbatim) + " s>> " + children->at(1)->expPrint(false, verbatim);
        if (parNeeded) {
            res = std::string("(") + res + ")";
        }
        return res;
    }
    else if (op == SLSHL) {
        std::string res = children->at(0)->expPrint(false, verbatim) + " s<< " + children->at(1)->expPrint(false, verbatim);
        if (parNeeded) {
            res = std::string("(") + res + ")";
        }
        return res;
    }

    else if (op == GMUL) {
        std::string res = "GMul(";
        for (int32_t i = 0; i < (int32_t) children->size() - 1; i += 1) {
            res += children->at(i)->expPrint(false, verbatim) + ", ";
        }
        res += children->at(children->size() - 1)->expPrint(false, verbatim) + ")";
        return res;
    }
    else if (op == GPOW) {
        return std::string("GPow(") + children->at(0)->expPrint(false, verbatim) + ", " + children->at(1)->expPrint(false, verbatim) + ")";
    }
    else if (op == GLOG) {
        return std::string("GLog(") + children->at(0)->expPrint(false, verbatim) + ")";
    }
    else if (op == GEXP) {
        return std::string("GExp(") + children->at(0)->expPrint(false, verbatim) + ")";
    }

    std::string res = "";
    for (int32_t idx = 0; idx < (int32_t) children->size(); idx += 1) {
        res += children->at(idx)->expPrint(true, verbatim);
        if (idx != (int32_t) children->size() - 1) {
            res += std::string(" ") + Node::op2strOp(op) + " ";
        }
    }
    if (parNeeded) {
        res = std::string("(") + res + ")";
    }
    return res;
}



std::ostream& operator<<(std::ostream &strm, const Node & n) {
    return strm << n.toString();
}




Node & Symb(const char * symb, char symbType, int32_t width, int32_t nbShares, int32_t shareNum, Node * origSecret, Node * pseudoShareEq) {
    std::string s(symb);
    for (char ch: Node::forbiddenChars) {
        if (s.find(ch) != std::string::npos) {
            std::cerr << "*** Error: symbol name (" << s << ") cannot contain the '" << ch << "' character" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    if (Node::symb2node.contains(s)) {
        std::cerr << "*** Error: symbol " << s << " has already been defined" << std::endl;
        exit(EXIT_FAILURE);
    }
    return SymbInternal(s, symbType, width, nbShares, shareNum, origSecret, pseudoShareEq);
}


Node & SymbInternal(std::string symb, char symbType, int32_t width, int32_t nbShares, int32_t shareNum, Node * origSecret, Node * pseudoShareEq) {
    // FIXME: check width ?
    if (Node::symb2node.contains(symb)) {
        return *Node::symb2node[symb];
    }
    else {
        Node & n = Node::SymbNode(symb, symbType, width, nbShares, shareNum, origSecret, pseudoShareEq);
        Node::symb2node[symb] = &n;
        return n;
    }
}


Node & SymbInternal(std::string symb, char symbType, int32_t width) {
    return SymbInternal(symb, symbType, width, 0, 0, NULL, NULL);
}


Node & Const(uint64_t * cst, int32_t nlimbs, int32_t width) {
    if (nlimbs == 1) {
        return Const(cst[0], width);
    }
    return Node::ConstNode(cst, nlimbs, width);
} 


Node & Const(uint64_t cst, int32_t width, bool extendMSB) {
    auto makeConstNode = [](uint64_t cst, int32_t nbBits) -> Node & {
        if (Node::cst2node.contains(nbBits)) {
            if (Node::cst2node[nbBits].contains(cst)) {
                return *Node::cst2node[nbBits][cst];
            }
            else {
                Node & n = Node::ConstNode(cst, nbBits);
                Node::cst2node[nbBits][cst] = &n;
                return n;
            }
        }
        else {
            Node::cst2node[nbBits] = std::map<uint64_t, Node *>();
            Node & n = Node::ConstNode(cst, nbBits);
            Node::cst2node[nbBits][cst] = &n;
            return n;
        }
    };

    // We want to accept constants in the interval [-2^(width - 1); 2^width - 1]
    // if the width is 64, the constant can necessarily be represented
    // if the width is less and the higher bit is 0, the constant is a positive value
    // if the width is less and the higher bit is 1, either the constant is a positive value greater than 0x8000000000000000
    // or a negative number. As we cannot know, we suppose it is a negative value since it is the only way to deal with them
    // In that case, we check that this negative value can be represented on width bits, and compute the associated positive value
    // Note: value -4 on 8 bits is encoded as 0xFC (= 252) in the cst field, similarly to the python implementation,
    //       whereas the value passed as parameter is 0xFFFFFFFFFFFFFFFC
    if (width > 64) {
        return Node::ConstNode(cst, width, extendMSB);
    }
    else if (width == 64) {
        return makeConstNode(cst, width);
    }
    else if ((cst & 0x8000000000000000) == 0) {
        // width < 64 and positive
        if (bit_width(cst) > width) {
            std::cerr << "*** Error: constant " << cst << " cannot be coded on " << width << " bits" << std::endl;
            std::cerr << "bit_width(" << cst << ") = " << bit_width(cst) << std::endl;
            assert(false);
        }
        return makeConstNode(cst, width);
    }
    else {
        // width < 64 and negative
        if ((int64_t) cst < -(1LL << (width - 1))) {
            std::cerr << "*** Error: constant " << cst << " cannot be coded on " << width << " bits" << std::endl;
            assert(false);
        }
        // Storing positive value
        cst = cst % (1ULL << width);
        return makeConstNode(cst, width);
    }
}


Node & Str(const std::string & s) {
    if (Node::str2node.contains(s)) {
        return *Node::str2node[s];
    }
    else {
        Node & n = Node::StrNode(s);
        Node::str2node[s] = &n;
        return n;
    }
}


Node & LShR(Node & child, Node & shval) {
    if (shval.nature != CONST) {
        return Node::OpNode(SLSHR, {&child, &shval});
    }
    return LShR(child, shval.cst[0]);
}


Node & LShR(Node & child, int32_t shval) {
    int32_t width = child.width;
    if (shval >= width) {
        std::cout << "*** Warning: shift value (" << shval << ") >= bit width of expression (" << width << ")" << std::endl;
    }

    if (propagateCstOnBuild() && child.nature == CONST) {
        int32_t nlimbs = child.nlimbs;
        uint64_t res[nlimbs];
        int32_t limb_shift = shval / 64;
        int32_t limb_rem = shval % 64;
        if (limb_rem == 0) {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                if (i + limb_shift >= nlimbs) {
                    res[i] = 0ULL;
                }
                else {
                    res[i] = child.cst[i + limb_shift];
                }
            }
        }
        else {
            for (int32_t i = 0; i < nlimbs; i += 1) {
                if (i + limb_shift >= nlimbs) {
                    res[i] = 0ULL;
                }
                else if (i + limb_shift == nlimbs - 1) {
                    res[i] = child.cst[nlimbs - 1] >> limb_rem;
                }
                else {
                    res[i] = child.cst[i + limb_shift] >> limb_rem | child.cst[i + limb_shift + 1] << (64 - limb_rem);
                }
            }
        }
        if (width % 64 != 0) {
            res[nlimbs - 1] = res[nlimbs - 1] & ((1ULL << (width % 64)) - 1);
        }
        return Const(res, nlimbs, width);
    }

    Node & sh = Const((uint64_t) shval, bit_width((uint64_t) shval));

    return Node::OpNode(LSHR, {&child, &sh});
}


Node & RotateRight(Node & child, Node & shval) {
    if (shval.nature != CONST) {
        std::cerr << "***Error: Second operand of a RotateRight operation can only be a constant" << std::endl;
        exit(EXIT_FAILURE);
    }
    return RotateRight(child, shval.cst[0]);
}


Node & RotateRight(Node & child, int32_t shval) {
    int32_t width = child.width;
    if (shval >= width) {
        std::cout << "*** Warning: shift value (" << shval << ") >= bit width of expression (" << width << ")" << std::endl;
    }

    return Concat(Extract(shval - 1, 0, child), Extract(width - 1, shval, child));
}



Node & ConstNodeFromConcat(const std::vector<Node *> & children) {
    int32_t width = 0;
    for (int32_t i = 0; i < (int32_t) children.size(); i += 1) {
        assert(children[i]->nature == CONST);
        width += children[i]->width;
    }
    int32_t nbLimbs = width / 64;
    if (nbLimbs * 64 != width) {
        nbLimbs += 1;
    }

    uint64_t res[nbLimbs] = {0};
    int32_t limbIdx = 0;
    int32_t bitIdx = 0;
    for (int32_t i = 0; i < (int32_t) children.size(); i += 1) {
        Node * child = children[i];
        int32_t remBits = child->width;
        for (int32_t j = 0; j < child->nlimbs; j += 1) {
            if (remBits >= 64) {
                if (bitIdx == 0) {
                    res[limbIdx] = child->cst[j];
                    limbIdx += 1;
                }
                else {
                    res[limbIdx] |= child->cst[j] << bitIdx;
                    res[limbIdx + 1] = child->cst[j] >> (64 - bitIdx);
                    limbIdx += 1;
                }
                remBits -= 64;
            }
            else {
                if (bitIdx + remBits > 64) {
                    res[limbIdx] |= child->cst[j] << bitIdx;
                    remBits -= 64 - bitIdx;
                    res[limbIdx + 1] = child->cst[j] >> (64 - bitIdx);
                    bitIdx = remBits;
                    limbIdx += 1;
                    remBits = 0;
                }
                else {
                    res[limbIdx] |= child->cst[j] << bitIdx;
                    if (bitIdx + remBits == 64) {
                        limbIdx += 1;
                        bitIdx = 0;
                    }
                    else {
                        bitIdx += remBits;
                    }
                    remBits = 0;
                }
            }
        }
        assert(remBits == 0);
    }
    return Const(res, nbLimbs, width);
}


Node & Concat(const std::vector<Node *> & children) {
    if (children.size() == 1) {
        return *children[0];
    }

    if (propagateCstOnBuild()) {
        bool allChildrenCst = true;
        for (const auto & child: children) {
            if (child->nature != CONST) {
                allChildrenCst = false;
                break;
            }
        }
        if (allChildrenCst) {
            return ConstNodeFromConcat(children);
        }
    }

    std::vector<Node *> nodeChildren(children.begin(), children.end());
    return Node::OpNode(CONCAT, nodeChildren);
}

/*
std::vector<Node *> variableArgs;
template <typename T> Node & Concat(T t) {
    variableArgs.push_back(&t);
    return Concat(variableArgs);
}

template<typename T, typename... Args>
Node & Concat(T t, Args... args) {
    static int i = 0;
    i += 1;
    variableArgs.push_back(&t);
    Node & res = Concat(args...);
    i -= 1;
    if (i == 0) {
        variableArgs.clear();
    }
    return res;
}
*/

Node & Concat(Node & n0, Node & n1) {
    return Concat({&n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2) {
    return Concat({&n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3) {
    return Concat({&n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4) {
    return Concat({&n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5) {
    return Concat({&n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6) {
    return Concat({&n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7) {
    return Concat({&n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8) {
    return Concat({&n8, &n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9) {
    return Concat({&n9, &n8, &n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10) {
    return Concat({&n10, &n9, &n8, &n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11) {
    return Concat({&n11, &n10, &n9, &n8, &n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11, Node & n12) {
    return Concat({&n12, &n11, &n10, &n9, &n8, &n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11, Node & n12, Node & n13) {
    return Concat({&n13, &n12, &n11, &n10, &n9, &n8, &n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11, Node & n12, Node & n13, Node & n14) {
    return Concat({&n14, &n13, &n12, &n11, &n10, &n9, &n8, &n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}

Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11, Node & n12, Node & n13, Node & n14, Node & n15) {
    return Concat({&n15, &n14, &n13, &n12, &n11, &n10, &n9, &n8, &n7, &n6, &n5, &n4, &n3, &n2, &n1, &n0});
}


/* Used also by simplify */
/* FIXME: how to limit to the scope to internal uses? */
Node & ConstNodeFromExtract(int32_t msb, int32_t lsb, const Node & n) {
    assert(n.nature == CONST);
    if (n.nlimbs == 1) {
        if (msb - lsb + 1 == 64) {
            return Const(n.cst[0], 64);
        }
        else {
            return Const((n.cst[0] >> lsb) & ((1ULL << (msb - lsb + 1)) - 1), msb - lsb + 1);
        }
    }
    else {
        int32_t width = msb - lsb + 1;
        int32_t nbLimbs = width / 64;
        if (nbLimbs * 64 != width) {
            nbLimbs += 1;
        }

        uint64_t res[nbLimbs];
        int32_t limbIdx = lsb / 64;
        int32_t bitIdx = lsb % 64;
        int32_t remBits = width;
        for (int32_t i = 0; i < nbLimbs; i += 1) {
            if (bitIdx == 0) {
                res[i] = n.cst[limbIdx];
                limbIdx += 1;
                if (remBits < 64) {
                    res[i] &= ((1ULL << remBits) - 1);
                    remBits = 0;
                }
                else {
                    remBits -= 64;
                }
            }
            else {
                if (remBits + bitIdx > 64) {
                    res[i] = n.cst[limbIdx] >> bitIdx;
                    res[i] |= n.cst[limbIdx + 1] << (64 - bitIdx);
                    limbIdx += 1;
                    if (remBits < 64) {
                        res[i] &= ((1ULL << remBits) - 1);
                        remBits = 0;
                    }
                    else {
                        remBits -= 64;
                    }
                }
                else {
                    res[i] = (n.cst[limbIdx] >> bitIdx) & ((1ULL << remBits) - 1);
                }
            }
        }
        return Const(res, nbLimbs, width);
    }
}



Node & Extract(Node & msbNode, Node & lsbNode, Node & child) {
    if (msbNode.nature != CONST || lsbNode.nature != CONST) {
        std::cerr << "*** Error: msb and lsb parameters of Extract must be integer constants" << std::endl;
        exit(EXIT_FAILURE);
    }

    int32_t msb = msbNode.cst[0];
    int32_t lsb = lsbNode.cst[0];

    if (msb < lsb) {
        std::cerr << "*** Error: msb must be greater than or equal to lsb" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (msb < 0 or msb >= child.width or lsb < 0 or lsb >= child.width) {
        std::cerr << "*** Error: msb and lsb parameters must be comprised between 0 and " << (child.width - 1) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (propagateCstOnBuild() and child.nature == CONST) {
        return ConstNodeFromExtract(msb, lsb, child);
    }

    if (msb == lsb and child.extractBit[lsb] != NULL) {
        return *child.extractBit[lsb];
    }

    Node & n = Node::OpNode(EXTRACT, {&msbNode, &lsbNode, &child});
    if (msb == lsb) {
        child.extractBit[lsb] = &n;
    }

    return n;
}


Node & Extract(int32_t msb, int32_t lsb, Node & child) {
    Node & msbNode = Const((uint64_t) msb, bit_width((uint64_t) msb));
    Node & lsbNode = Const((uint64_t) lsb, bit_width((uint64_t) lsb));
    return Extract(msbNode, lsbNode, child);
}


Node & ConstNodeFromZeroExt(int32_t numZeros, Node & n) {
    assert(n.nature == CONST);
    int32_t width = n.width + numZeros;
    int32_t nbLimbs = width / 64;
    if (nbLimbs * 64 != width) {
        nbLimbs += 1;
    }

    uint64_t res[nbLimbs];
    int32_t remBits = numZeros;
    int32_t bitIdx = n.width % 64;

    for (int32_t i = 0; i < n.nlimbs - 1; i += 1) {
        res[i] = n.cst[i];
    }
    if (bitIdx == 0) {
        res[n.nlimbs - 1] = n.cst[n.nlimbs - 1];
    }
    else if (bitIdx + remBits < 64) {
        res[n.nlimbs - 1] = n.cst[n.nlimbs - 1];
        remBits = 0;
    }
    else {
        res[n.nlimbs - 1] = n.cst[n.nlimbs - 1];
        remBits -= (64 - bitIdx);
    }

    int32_t limbIdx = n.nlimbs;
    while (remBits >= 64) {
        res[limbIdx] = 0ULL;
        limbIdx += 1;
        remBits -= 64;
    }

    if (remBits > 0) {
        res[limbIdx] = 0ULL;
    }

    return Const(res, nbLimbs, width);
}


Node & ZeroExt(Node & numZeros, Node & child) {
    if (numZeros.nature != CONST) {
        std::cerr << "*** Error: numZeros parameter of ZeroExt must be an integer constant" << std::endl;
        exit(EXIT_FAILURE);
    }
    return ZeroExt(numZeros.cst[0], child);
}


Node & ZeroExt(int32_t numZeros, Node & child) {
    if (numZeros <= 0) {
        std::cerr << "*** Error: numZeros parameter of ZeroExt must be greater than 0" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (propagateCstOnBuild() and child.nature == CONST) {
        return ConstNodeFromZeroExt(numZeros, child);
    }

    return Node::OpNode(CONCAT, {&child, &Const(0ULL, numZeros)});
}


Node & SignExt(Node & numSignBits, Node & child) {
    if (numSignBits.nature != CONST) {
        std::cerr << "*** Error: numSignBits parameter of SignExt must be an integer constant" << std::endl;
        exit(EXIT_FAILURE);
    }
    return SignExt(numSignBits.cst[0], child);
}


Node & SignExt(int32_t numSignBits, Node & child) {
    if (numSignBits <= 0) {
        std::cerr << "*** Error: numSignBits parameter of SignExt must be greater than 0" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (propagateCstOnBuild() and child.nature == CONST) {
        int32_t width = child.width + numSignBits;
        int32_t nbLimbs = width / 64;
        if (nbLimbs * 64 != width) {
            nbLimbs += 1;
        }

        uint64_t res[nbLimbs];
        int32_t li = (child.width - 1) / 64;
        int32_t bi = (child.width - 1) % 64;
        uint64_t v = child.cst[li] >> bi;
        int64_t m = (int64_t) v << 63;
        int32_t remBits = numSignBits;
        int32_t bitIdx = child.width % 64;

        for (int32_t i = 0; i < child.nlimbs - 1; i += 1) {
            res[i] = child.cst[i];
        }
        if (bitIdx == 0) {
            res[child.nlimbs - 1] = child.cst[child.nlimbs - 1];
        }
        else if (bitIdx + remBits < 64) {
            res[child.nlimbs - 1] = child.cst[child.nlimbs - 1] | (uint64_t) (m >> (remBits)) >> (64 - bitIdx - remBits);
            remBits = 0;
        }
        else {
            res[child.nlimbs - 1] = child.cst[child.nlimbs - 1] | (m >> (64 - bitIdx));
            remBits -= (64 - bitIdx);
        }

        int32_t limbIdx = child.nlimbs;
        while (remBits >= 64) {
            res[limbIdx] = (uint64_t) (m >> 63);
            limbIdx += 1;
            remBits -= 64;
        }

        if (remBits > 0) {
            res[limbIdx] = (m >> 63) & ((1ULL << remBits) - 1);
        }

        return Const(res, nbLimbs, width);
    }

    std::vector<Node *> childrenList;
    childrenList.push_back(&child);
    Node & e = Extract(child.width - 1, child.width - 1, child);
    for (int32_t i = 0; i < numSignBits; i += 1) {
        childrenList.push_back(&e);
    }
    return Node::OpNode(CONCAT, childrenList);
}


Node & GMul(std::vector<Node *> children) {
    assert(children.size() >= 2);
    bool allChildrenCst = true;
    for (auto c: children) {
        if (c->nature != CONST) {
            allChildrenCst = false;
            break;
        }
    }
    if (propagateCstOnBuild() and allChildrenCst) {
        Node * res = children[0];
        for (int32_t idx = 1; idx < (int32_t) children.size(); idx += 1) {
            res = &gmul(*res, *children[idx]);
        }
        return *res;
    }
    return Node::OpNode(GMUL, children);
}


/*
template <typename T> Node & GMul(T t) {
    variableArgs.push_back(&t);
    return GMul(variableArgs);
}

template<typename T, typename... Args>
Node & GMul(T t, Args... args) {
    static int i = 0;
    i += 1;
    variableArgs.push_back(&t);
    Node & res = GMul(args...);
    i -= 1;
    if (i == 0) {
        variableArgs.clear();
    }
    return res;
}
*/

Node & GMul(Node & n0, Node & n1) {
    return GMul({&n0, &n1});
}

Node & GMul(Node & n0, Node & n1, Node & n2) {
    return GMul({&n0, &n1, &n2});
}

Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3) {
    return GMul({&n0, &n1, &n2, &n3});
}

Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4) {
    return GMul({&n0, &n1, &n2, &n3, &n4});
}

Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5) {
    return GMul({&n0, &n1, &n2, &n3, &n4, &n5});
}

Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6) {
    return GMul({&n0, &n1, &n2, &n3, &n4, &n5, &n6});
}

Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7) {
    return GMul({&n0, &n1, &n2, &n3, &n4, &n5, &n6, &n7});
}



Node & GPow(Node & c0, Node & c1) {
    if (propagateCstOnBuild() and c0.nature == CONST and c1.nature == CONST) {
        return gpow(c0, c1);
    }
    return Node::OpNode(GPOW, {&c0, &c1});
}


Node & GLog(Node & child) {
    if (propagateCstOnBuild() and child.nature == CONST) {
        return glog(child);
    }
    return Node::OpNode(GLOG, {&child});
}


Node & GExp(Node & child) {
    if (propagateCstOnBuild() and child.nature == CONST) {
        return gexp(child);
    }
    return Node::OpNode(GEXP, {&child});
}




Node & imul(Node & n0, Node & n1) {
    assert(n0.width == n1.width);
    return Const(imulInt(n0.cst[0], n1.cst[0], n0.width), n0.width);
}


uint64_t imulInt(uint64_t a, uint64_t b, int32_t w) {
    assert(w <= 64);
    return w == 64 ? (a * b) : (a * b) % (1ULL << w);
}


Node & ipow(Node & n0, Node & n1) {
    assert(n0.width == n1.width);
    return Const(ipowInt(n0.cst[0], n1.cst[0], n0.width), n0.width);
}


uint64_t ipowInt(uint64_t a, uint64_t b, int32_t w) {
    assert(w <= 64);
    return w == 64 ? (a * b) : (a * b) % (1ULL << w); // FIXME a ** b and not a * b
}



Node & gmul(Node & n0, Node & n1) {
    assert(n0.width == 8 and n1.width == 8);
    return Const(gmulInt(n0.cst[0], n1.cst[0]), 8);
}


uint64_t gmulInt(uint64_t a, uint64_t b) {
    assert(a >= 0 and a < 256);
    assert(b >= 0 and b < 256);
    uint64_t p = 0;
    for (int32_t i = 0; i < 8; i += 1) {
        p = (((b & 1) * 0xff) & a) ^ p;
        a = ((a << 1) & 0xff) ^ ((((a & 0x80) >> 7) * 0xff) & Node::gfIrr);
        b >>= 1;
    }
    assert(p < 256);
    return p;
}


Node & gpow(Node & n0, Node & n1) {
    assert(n0.width == 8 and n1.width == 8);
    return Const(gpowInt(n0.cst[0], n1.cst[0]), 8);
}


uint64_t gpowInt(uint64_t a, uint64_t b) {
    assert(a >= 0 and a < 256);
    assert(b >= 0 and b < 256);
    uint64_t p = 1;
    for (int32_t i = 0; i < (int32_t) b; i += 1) {
        p = gmulInt(a, p);
    }
    assert(p < 256);
    return p;
}


Node & gexp(Node & n) {
    assert(n.width == 8);
    return Const(gexpInt(n.cst[0]), 8);
}


uint64_t gexpInt(uint64_t a) {
    assert(a >= 0 and a < 256);
    return gpowInt(Node::gfBase, a);
}


Node & glog(Node & n) {
    assert(n.width == 8);
    return Const(glogInt(n.cst[0]), 8);
}


uint64_t glogInt(uint64_t a) {
    // ...
    if (a == 0) {
        return 0;
    }
    if (a == 1) {
        return 0;
    }
    uint64_t v = a;
    while (true) {
        uint64_t e = gexpInt(v);
        if (e == a) {
            return v;
        }
        v = e;
    }
}


bool isZero(uint64_t * v, int32_t width) {
    int32_t nbLimbs = width / 64;
    if (nbLimbs * 64 != width) {
        nbLimbs += 1;
    }
    for (int32_t i = 0; i < nbLimbs; i += 1) {
        if (v[i] != 0) {
            return false;
        }
    }
    return true;
}


bool isAllOne(uint64_t * v, int32_t width) {
    int32_t nbLimbs = width / 64;
    if (nbLimbs * 64 != width) {
        nbLimbs += 1;
    }
    for (int32_t i = 0; i < nbLimbs - 1; i += 1) {
        if (v[i] != 0xFFFFFFFFFFFFFFFFULL) {
            return false;
        }
    }
    if (width % 64 != 0) {
        return (v[nbLimbs - 1] == ((1ULL << (width % 64)) - 1));
    }
    else {
        return v[nbLimbs - 1] == 0xFFFFFFFFFFFFFFFFULL;
    }
}



/*
void expandConcat() {
    Node & p = Symb("p", 'P', 1, 0, NULL, NULL);
    Node & n = Concat(p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p, p);
    (void) n;
}
*/



