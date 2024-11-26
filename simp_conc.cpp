/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#include <cassert>

#include "node.hpp"

static int32_t currBit;
static int32_t nextCurrBit;
static int32_t varWidth;
static std::map<Node *, Node *> m;
static std::set<Node *> processedNodes;
static bool abortAll;


static void traverseExpRec(Node & n) {
    if (n.nature != OP) {
        return;
    }
    if (n.op == EXTRACT and n.children->at(2)->nature == SYMB) {
        if (n.children->at(0)->nature != CONST or n.children->at(1)->nature != CONST) {
            abortAll = true;
            return;
        }
        if (n.children->at(1)->cst[0] != (uint64_t) currBit) {
            abortAll = true;
            return;
        }
        if (varWidth == -1) {
            varWidth = n.children->at(2)->width;
        }
        else if (n.children->at(2)->width != varWidth) {
            abortAll = true;
            return;
        }
        if (nextCurrBit == -1) {
            nextCurrBit = (int32_t) n.children->at(0)->cst[0];
        }
        else if (nextCurrBit != (int32_t) n.children->at(0)->cst[0]) {
            abortAll = true;
        }
        return;
    }
    for (const auto & child : *n.children) {
        if (not processedNodes.contains(child)) {
            processedNodes.insert(child);
            traverseExpRec(*child);
            if (abortAll) {
                break;
            }
        }
    }
}


static void traverseExp(Node & n) {
    processedNodes.clear();
    traverseExpRec(n);
}


static Node & replaceExtractsRec(Node & n) {
    if (n.nature != OP) {
        return n;
    }
    if (n.op == EXTRACT and n.children->at(2)->nature == SYMB) {
        Node & e = Extract(0, 0, *n.children->at(2));
        m[&n] = &e;
        return e;
    }
    std::vector<Node *> children;
    for (const auto & child : *n.children) {
        if (m.contains(child)) {
            children.push_back(m[child]);
        }
        else {
            Node & newChild = replaceExtractsRec(*child);
            children.push_back(&newChild);
        }
    }
    Node * retNode = NULL;
    if (children == *n.children) {
        retNode = &n;
    }
    else {
        assert(n.op != EXTRACT); // deal with it when time comes..
        retNode = &Node::OpNode(n.op, children);
    }
    m[&n] = retNode;
    return *retNode;
}


static Node & replaceExtracts(Node & n) {
    m.clear();
    return replaceExtractsRec(n);
}


static Node & buildEquivNodeRec(Node & n) {
    if (n.nature != OP) {
        return n;
    }
    if (n.op == EXTRACT and n.children->at(2)->nature == SYMB) {
        Node & e = *n.children->at(2);
        m[&n] = &e;
        return e;
    }
    std::vector<Node *> children;
    for (const auto & child : *n.children) {
        if (m.contains(child)) {
            children.push_back(m[child]);
        }
        else {
            Node & newChild = buildEquivNodeRec(*child);
            children.push_back(&newChild);
        }
    }
    assert(n.op != EXTRACT);
    Node & retNode = Node::OpNode(n.op, children);
    m[&n] = &retNode;
    return retNode;
}


static Node & buildEquivNode(Node & n) {
    m.clear();
    return buildEquivNodeRec(n);
}


// Like equivalence but does not simplify nodes first
static bool rawEquivalence(Node & node0, Node & node1) {
    for (int32_t i = 0; i < 4; i += 1) {
        if (node0.h[i] != node1.h[i]) {
            return false;
        }
    }
    return true;
}



Node * extendedMergeConcatExtract(std::vector<Node *> & children, bool * ok) {

    abortAll = false;
    currBit = 0;
    varWidth = -1;

    bool firstChild = true;
    Node * refReplacedChild;
    for (const auto & child : children) {
        nextCurrBit = -1;
        traverseExp(*child);
        if (abortAll || nextCurrBit == -1) {
            *ok = false;
            return NULL;
        }
        if (firstChild) {
            firstChild = false;
            refReplacedChild = &replaceExtracts(*child);
        }
        else {
            Node & replacedChild = replaceExtracts(*child);
            if (not rawEquivalence(*refReplacedChild, replacedChild)) {
                *ok = false;
                return NULL;
            }
        }
        currBit = nextCurrBit + 1;
    }
    if (currBit != varWidth) {
        *ok = false;
        return NULL;
    }

    *ok = true;
    Node & retNode = buildEquivNode(*children[0]);
    return &retNode;
}


// FIXME: set temporary mode and delete temporary nodes?

