/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#include <vector>
#include <assert.h>
#include <algorithm>


#include "simplify.hpp"
#include "config.hpp"
#include "node.hpp"
#include "arrayexp.hpp"
#include "simp_conc.hpp"
#include "simp_rules.hpp"



static Node & simplifyCore(Node & node, bool propagateExtractInwards, bool useSingleBitVariables);


/*
 * To trace calls, uncomment this code and replace calls to simplifyCore with calls to simplifyCoreWrapper


#define DEBUG
#ifdef DEBUG
static int offset = 0;
#endif


static Node & simplifyCoreWrapper(Node & node, bool pei, bool usbv) {
    #ifdef DEBUG
        for (int i = 0; i < offset; i += 1) {
            std::cout << "   ";
        }
        offset += 1;
        std::cout << "Simplifying node: " << node.verbatimPrint() << std::endl;
    #endif
    Node & res = simplifyCore(node, pei, usbv);
    #ifdef DEBUG
        offset -= 1;
        for (int i = 0; i < offset; i += 1) {
            std::cout << "   ";
        }
        std::cout << "Result: " << res.verbatimPrint() << std::endl;
    #endif
    return res;
}


*/


// Trying to merge current node with its children if associative and children have the same op 
static bool mergeWithChildrenIfPossible(NodeOp op, std::vector<Node *> & children) {
    assert(children.size() != 0);
    if (Node::associativeOps.contains(op)) {
        bool merge = false;
        for (const auto & child : children) {
            if (child->op == op) {
                merge = true;
                break;
            }
        }
        if (merge) {
            std::vector<Node *> newChildren;
            for (const auto & child : children) {
                if (child->op == op) {
                    newChildren.insert(newChildren.end(), child->children->begin(), child->children->end());
                }
                else {
                    newChildren.push_back(child);
                }
            }
            children = newChildren;
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}


// Concat(a, b) ^ Concat(c, d) ^ Concat(e, f) ^ g -> Concat(a ^ c ^ e, b ^ d ^ f) ^ g
// Note regarding the '+': carry is ignored (not propagated) from an expression to the next
// This is consistent with the modulo semantic of an expression, but it might differ from a low level implementation
// Note: Concat(a8, b8) ^ Concat(c8, d8) ^ Concat(u4, v12) ^ Concat(w4, x12)
//       will not be simplified to Concat(a8 ^ c8, b8 ^ d8) ^ Concat(u4 ^ w4, v12 ^ x12)
//       This case can be added if it is encountered
// FIXME: propagate in verif_msi
static bool mergeConcatChildren(NodeOp op, std::vector<Node *> & children, NodeOp * newOp, bool usbv) {
    std::vector<int> concatChildrenIdx;
    bool concatPresent = false;
    for (int i = 0; i < (int) children.size(); i += 1) {
        if (children[i]->op == CONCAT or children[i]->nature == CONST) {
            concatPresent |= children[i]->op == CONCAT;
            concatChildrenIdx.push_back(i);
        }
    }
    int numConcatChildren = concatChildrenIdx.size();

    if (concatPresent and numConcatChildren >= 1 and usbv) {
        // Decomposing "symbolic nodes" for allowing them to merge
        bool modified = false;
        for (int i = 0; i < (int) children.size(); i += 1) {
            if (children[i]->op == ARRAY or children[i]->op == SLSHR or children[i]->op == SLSHL or children[i]->op == SASHR or children[i]->op == PLUS or children[i]->op == EXTRACT) {
                // Not using getBitDecomposition because an array node will be simplified to the array node itself,
                // removing the outer Concat
                std::vector<Node *> l;
                for (int32_t b = 0; b < children[i]->width; b += 1) {
                    l.push_back(&simplifyCore(Extract(b, b, *children[i]), true, true));
                }
                children[i] = &Concat(l);

                modified = true;
            }
        }
        if (modified) {
            // Recomputing indexes
            concatChildrenIdx.clear();
            for (int i = 0; i < (int) children.size(); i += 1) {
                if (children[i]->op == CONCAT or children[i]->nature == CONST) {
                    concatPresent |= children[i]->op == CONCAT;
                    concatChildrenIdx.push_back(i);
                }
            }
            numConcatChildren = concatChildrenIdx.size();
        }
    }


    if (concatPresent and numConcatChildren > 1) {
        bool removedChild[numConcatChildren];
        std::fill_n(removedChild, numConcatChildren, false);
        std::vector<int> bitsTaken;
        {
            int currChild[numConcatChildren];
            int currChildSize[numConcatChildren];
            for (int i = 0; i < numConcatChildren; i += 1) {
                Node * child = children[concatChildrenIdx[i]];
                if (child->nature == CONST) {
                    currChildSize[i] = child->width;
                }
                else {
                    currChild[i] = child->children->size() - 1;
                    currChildSize[i] = child->children->at(currChild[i])->width;
                }
            }

            int remainingBits = children[0]->width;

            while (true) {
                int nbBitsTaken = remainingBits;
                int selChildIdx = -1;

                // Initializing width of segment with the first encountered non constant child
                for (int i = 0; i < numConcatChildren; i += 1) {
                    if (removedChild[i]) {
                        continue;
                    }
                    Node * child = children[concatChildrenIdx[i]];
                    if (child->op == CONCAT and child->children->at(currChild[i])->nature != CONST) {
                        selChildIdx = i;
                        nbBitsTaken = currChildSize[i];
                        break;
                    }
                }

                if (selChildIdx == -1) {
                    for (int i = 0; i < numConcatChildren; i += 1) {
                        if (removedChild[i]) {
                            continue;
                        }
                        if (currChildSize[i] < nbBitsTaken) {
                            nbBitsTaken = currChildSize[i];
                        }
                    }
                }
                else {
                    // Removing incompatible children with selected child
                    for (int i = 0; i < numConcatChildren; i += 1) {
                        if (removedChild[i]) {
                            continue;
                        }
                        Node * child = children[concatChildrenIdx[i]];
                        if (child->op == CONCAT) {
                            Node * concatChild = child->children->at(currChild[i]);
                            if (concatChild->nature != CONST && concatChild->width != nbBitsTaken) {
                                // a non constant node with different width cannot be merged
                                removedChild[i] = true;
                            }
                            else if (concatChild->nature == CONST && currChildSize[i] < nbBitsTaken) {
                                // +---------------
                                // |    a    | cst
                                // +---------------
                                //
                                // +---------------
                                // |  cst |   b
                                // +---------------
                                removedChild[i] = true;
                            }
                        }
                    }
                }


                bitsTaken.push_back(nbBitsTaken);

                remainingBits -= nbBitsTaken;
                if (remainingBits <= 0) {
                    assert(remainingBits == 0);
                    break;
                }

                for (int i = 0; i < numConcatChildren; i += 1) {
                    if (removedChild[i]) {
                        continue;
                    }
                    if (currChildSize[i] > nbBitsTaken) {
                        currChildSize[i] -= nbBitsTaken;
                    }
                    else {
                        assert(currChildSize[i] == nbBitsTaken);
                        currChild[i] -= 1;
                        currChildSize[i] = children[concatChildrenIdx[i]]->children->at(currChild[i])->width;
                    }
                }
            }
        }

        int numKeptConcatChildren = 0;
        for (int i = 0; i < numConcatChildren; i += 1) {
            numKeptConcatChildren += removedChild[i] ? 0 : 1;
        }

        // Computing constant values on segments for non removed children only
        if (numKeptConcatChildren > 1) {
            concatChildrenIdx.clear();
            std::vector<Node *> newChildren;
            int32_t concatChildIdx = 0;
            for (int i = 0; i < (int) children.size(); i += 1) {
                if (children[i]->nature == CONST or children[i]->op == CONCAT) {
                    if (!removedChild[concatChildIdx]) {
                        concatChildrenIdx.push_back(i);
                    }
                    else {
                        newChildren.push_back(children[i]);
                    }
                    concatChildIdx += 1;
                }
                else {
                    newChildren.push_back(children[i]);
                }
            }

            int currChildIdx[numKeptConcatChildren];
            int currChildSize[numKeptConcatChildren];
            for (int i = 0; i < numKeptConcatChildren; i += 1) {
                Node * child = children[concatChildrenIdx[i]];
                if (child->nature == CONST) {
                    currChildSize[i] = child->width;
                }
                else {
                    currChildIdx[i] = child->children->size() - 1;
                    currChildSize[i] = child->children->at(currChildIdx[i])->width;
                }
            }


            std::vector<Node *> cstChildren;
            std::vector<Node *> otherChildren;
            std::vector<Node *> concatChildren;
            for (const auto & bt : bitsTaken) {
                cstChildren.clear();
                otherChildren.clear();
                for (int i = 0; i < numKeptConcatChildren; i += 1) {
                    Node * child = children[concatChildrenIdx[i]];
                    Node * currChild;
                    if (child->nature == CONST) {
                        currChild = child;
                    }
                    else {
                        currChild = child->children->at(currChildIdx[i]);
                    }
                    if (currChild->nature == CONST) {
                        cstChildren.push_back(&Extract(currChildSize[i] - 1, currChildSize[i] - bt, *currChild));
                    }
                    else {
                        otherChildren.push_back(currChild);
                        assert(currChild->width == bt);
                    }

                    if (currChildSize[i] > bt) {
                        currChildSize[i] -= bt;
                    }
                    else if (child->op == CONCAT) {
                        currChildIdx[i] -= 1;
                        if (currChildIdx[i] >= 0) {
                            currChildSize[i] = children[concatChildrenIdx[i]]->children->at(currChildIdx[i])->width;
                        }
                    }
                }

                // QM FIXME: why simplify and not simplifyCore(., ?? , usbv) ?
                Node * cstNode = NULL;
                Node * opNode = NULL;
                if (cstChildren.size() == 1) {
                    cstNode = cstChildren[0];
                }
                else if (cstChildren.size() > 1) {
                    cstNode = &simplify(Node::OpNode(op, cstChildren));
                    assert(cstNode->nature == CONST);
                }
                if (otherChildren.size() == 1) {
                    opNode = otherChildren[0];
                }
                else if (otherChildren.size() > 1) {
                    opNode = &simplify(Node::OpNode(op, otherChildren));
                }
                if (cstNode == NULL) {
                    concatChildren.push_back(opNode);
                }
                else if (opNode == NULL) {
                    concatChildren.push_back(cstNode);
                }
                else {
                    concatChildren.push_back(&simplify(Node::OpNode(op, {cstNode, opNode})));
                }
            }

            std::reverse(concatChildren.begin(), concatChildren.end());
            if (newChildren.size() == 0) {
                *newOp = CONCAT;
                children = concatChildren;
                return true;
            }
            else {
                *newOp = op;
                Node & concatNode = simplify(Node::OpNode(CONCAT, concatChildren));
                children = newChildren;
                children.push_back(&concatNode);
                return true;
            }
        }
    }

    return false;
}


static Node & simplifyExtract(Node & node) {
    // No recursion in this function
    Node & child = *node.children->at(2);
    Node & msbNode = *node.children->at(0);
    Node & lsbNode = *node.children->at(1);
    int32_t msb = msbNode.cst[0];
    int32_t lsb = lsbNode.cst[0];
    
    if (child.nature == CONST) {
        return ConstNodeFromExtract(msb, lsb, child);
    }
    
    else if (child.op == SEXT || child.op == ZEXT) {
        assert(false);
        Node & extendNode = child;
        Node & gchild = *extendNode.children->at(1);
        if (msb <= gchild.width - 1) {
            // Remove SE or ZE
            if (lsb == 0 && msb == gchild.width - 1) {
                // +---------+-------------+
                // | Extend  |      e      |
                // +---------+-------------+
                //           \-- Extract --/
                return gchild;
            }
            else {
                // +------+----------------+
                // | Ext  |        e       |
                // +------+----------------+
                //           \- Extract -/
                return Extract(msb, lsb, gchild);
            }
        }
        else if (lsb == 0) {
            // We also have msb > gchild.width - 1
            // We can remove the Extract if we modify the Extension length
            // +---------------+-------+
            // | Extend        |   e   |
            // +---------------+-------+
            //           \-- Extract --/
            Node & extendNode = Node::ConstNodeAuto(node.width - gchild.width);
            return Node::OpNode(child.op, {&extendNode, &gchild});
        }
        else if (lsb > gchild.width - 1 && extendNode.op == ZEXT) {
            // +-----------------+-------+
            // | ZExt            |   e   |
            // +-----------------+-------+
            //   \-- Extract --/
            return Const(0, msb - lsb + 1);
        }
        else if (lsb >= gchild.width - 1 && extendNode.op == SEXT) {
            assert(extendNode.op == SEXT);
            if (gchild.width > 1) {
                // +-----------------+-------+
                // | SExt            |   e   |
                // +-----------------+-------+
                //   \-- Extract --/
                int32_t gchildMsb = gchild.width - 1;
                Node & newExtractNode = Extract(gchildMsb, gchildMsb, gchild);
                return SignExt(msb - lsb, newExtractNode);
            }
            else {
                return SignExt(msb - lsb, gchild);
            }
        }
        else {
            return node;
        }
    }

    else if (child.op == CONCAT) {
        Node & concatNode = child;
        int32_t startBitIdx = lsb;
        int32_t removedBitsOnRight = 0;
        int32_t i = 0;
        while (concatNode.children->at(i)->width <= startBitIdx) {
            Node & concatChild = *concatNode.children->at(i);
            int32_t concatChildWidth = concatChild.width;
            startBitIdx -= concatChildWidth;
            removedBitsOnRight += concatChildWidth;
            i += 1;
        }

        int32_t startChildIdx = i;
        int32_t endBitIdx = msb - removedBitsOnRight;

        i = startChildIdx;
        int32_t currBitIdx = concatNode.children->at(i)->width - 1;
        while (currBitIdx < endBitIdx) {
            i += 1;
            currBitIdx += concatNode.children->at(i)->width;
        }
        int32_t endChildIdx = i;

        if (startChildIdx == endChildIdx) {
            // Remove Concat
            if (startBitIdx == 0 && endBitIdx == concatNode.children->at(startChildIdx)->width - 1) {
                // Also Remove Extract
                return *concatNode.children->at(startChildIdx);
            }
            else {
                return Extract(endBitIdx, startBitIdx, *concatNode.children->at(startChildIdx));
            }
        }
        
        // Extract covers 2 children or more
        //std::vector<Node *>::reverse_iterator it = concatNode.children->rend();
        //std::vector<Node *> concatChildren(it - endChildIdx - 1, it - startChildIdx);
        std::vector<Node *>::iterator it = concatNode.children->begin();
        std::vector<Node *> concatChildren(it + startChildIdx, it + endChildIdx + 1);
        Node & newConcatNode = Concat(concatChildren);
        if (startBitIdx == 0 && endBitIdx == currBitIdx) {
            // Remove Extract
            return newConcatNode;
        }
        else {
            return Extract(endBitIdx, startBitIdx, newConcatNode);
        }
    }

    else if (child.op == EXTRACT) {
        // Extract(m, l, Extract(v, u, e)) -> Extract(m + u, l + u, e)
        Node & gLsbNode = *child.children->at(1);
        int32_t gLsb = (int32_t) gLsbNode.cst[0];
        Node & gchild = *child.children->at(2);
        return Extract(msb + gLsb, lsb + gLsb, gchild);
    }


    // Default case
    else {
        return node;
    }
}


static Node & defaultNode(Node & node, NodeOp op, const std::vector<Node *> & newChildren, bool modified) {
    if (modified) {
        if (op == EXTRACT) {
            return Extract(*newChildren[0], *newChildren[1], *newChildren[2]);
        }
        else {
            return Node::OpNode(op, newChildren);
        }
    }
    else {
        return node;
    }
}


static Node & getBitDecompositionVar(Node & node, int32_t msb, int32_t lsb);

static Node & getBitDecompositionVarSingleBit(Node & node, int32_t bit) {
    Node & extractNode = Extract(bit, bit, node);
    if (extractNode.simpEqUsbv != NULL) {
        //std::cout << "# SimpEqUsbv bit " << bit << " of node '" << node << "' is: " << *extractNode.simpEqUsbv << std::endl;
        return *extractNode.simpEqUsbv;
    }
    //std::cout << "# Calling SymbInternal for node " << node << " bit " << bit << std::endl;
    std::string s = *node.symb + "#" + std::to_string(bit);
    Node * res;
    if (node.symbType == 'A') {
        res = &SymbInternal(s, 'A', 1, node.nbShares, node.shareNum, &getBitDecompositionVar(*node.origSecret, bit, bit), &simplifyCore(Extract(bit, bit, *node.pseudoShareEq), true, true));
    }
    else {
        res = &SymbInternal(s, node.symbType, 1);
    }
    //std::cout << "# Setting simpEqUsbv bit " << bit << " of node '" << node << "' to: " << *res << std::endl;
    extractNode.simpEqUsbv = res;
    return *res;
}


static Node & getBitDecompositionVar(Node & node, int32_t msb, int32_t lsb) {
    assert(node.nature == SYMB);
    if (node.width == 1) {
        return node;
    }

    std::vector<Node *> newChildren0;
    //for (int i = msb; i > lsb - 1; i -= 1) {
    for (int i = lsb; i < msb + 1; i += 1) {
        Node & symb = getBitDecompositionVarSingleBit(node, i);
        newChildren0.push_back(&symb);
    }
    Node & be = Concat(newChildren0);
    return be;
}


static Node & getBitDecompositionVar(Node & node) {
    assert(node.nature == SYMB);
    if (node.simpEqUsbv != NULL) {
        //std::cout << "# Concat ext eq of '" << node << "': " << *node.simpEqUsbv << std::endl;
        return *node.simpEqUsbv;
    }
    Node * res;
    res = &getBitDecompositionVar(node, node.width - 1, 0);
    node.simpEqUsbv = res;
    return *res;
}


Node & getBitDecomposition(Node & node) {
    if (node.nature == CONST or node.nature == STR) {
        return node;
    }
    if (node.simpEqUsbv != NULL) {
        return *node.simpEqUsbv;
    }
    if (node.nature == SYMB) {
        // shortcut, not necessary
        return getBitDecompositionVar(node);
    }

    std::vector<Node *> l;
    for (int32_t b = 0; b < node.width; b += 1) {
        l.push_back(&Extract(b, b, node));
    }
    Node & conc = Concat(l);
    Node & simpConc = simplifyCore(conc, true, true);
    return simpConc;
}


bool factorize(NodeOp mulOp, NodeOp addOp, std::vector<Node *> & newChildren, int32_t width) {
    bool hasChanged = true;
    bool modified = false;
    while (hasChanged) {
        hasChanged = false;
        int32_t i = 0;
        while (i < (int32_t) newChildren.size()) {
            if (newChildren[i]->op == mulOp) {
                Node * firstMul = newChildren[i];
                int32_t j = 0;
                while (j < (int32_t) newChildren.size()) {
                    if (i == j) {
                        j += 1;
                        continue;
                    }
                    std::vector<Node *> * newGrandChildren = NULL;
                    if (newChildren[j]->op == mulOp) {
                        // case  GM(a, b) ^  GM(a, c) -> GM(a, (b ^ c))
                        // case  a * b    +  a * c    -> a *  (b + c)
                        // case  a &  b   |  a & c    -> a &  (b | c)
                        // case (a |  b)  & (a | c)   -> a |  (b & c)
                        // case (a &  b)  ^ (a & c)   -> a &  (b ^ c)
                        Node * secndMul = newChildren[j];
                        int32_t k = 0;
                        while (k < (int32_t) firstMul->children->size()) {
                            int32_t l = 0;
                            while (l < (int32_t) secndMul->children->size()) {
                                if (equivalence(*firstMul->children->at(k), *secndMul->children->at(l))) {
                                    hasChanged = true;
                                    Node * factor = firstMul->children->at(k);
                                    // Determining the add/or operands
                                    std::vector<Node *> xorLeftChildren = *firstMul->children;
                                    xorLeftChildren.erase(xorLeftChildren.begin() + k);
                                    Node * xorLeftNode;
                                    if (xorLeftChildren.size() == 1) {
                                        xorLeftNode = xorLeftChildren[0];
                                    }
                                    else {
                                        xorLeftNode = &Node::OpNode(mulOp, xorLeftChildren);
                                    }
                                    std::vector<Node *> xorRighChildren = *secndMul->children;
                                    xorRighChildren.erase(xorRighChildren.begin() + l);
                                    Node * xorRighNode;
                                    if (xorRighChildren.size() == 1) {
                                        xorRighNode = xorRighChildren[0];
                                    }
                                    else {
                                        xorRighNode = &Node::OpNode(mulOp, xorRighChildren);
                                    }

                                    // QM FIXME: what if usbv is used ?
                                    Node * xorNode = NULL;
                                    if (mulOp == GMUL) {
                                        assert(addOp == BXOR);
                                        xorNode = &simplify(*xorLeftNode ^ *xorRighNode);
                                    }
                                    else if (mulOp == IMUL) {
                                        assert(addOp == PLUS);
                                        xorNode = &simplify(*xorLeftNode + *xorRighNode);
                                    }
                                    else if (mulOp == BOR) {
                                        assert(addOp == BAND);
                                        xorNode = &simplify(*xorLeftNode & *xorRighNode);
                                    }
                                    else if (mulOp == BAND && addOp == BOR) {
                                        xorNode = &simplify(*xorLeftNode | *xorRighNode);
                                    }
                                    else if (mulOp == BAND && addOp == BXOR) {
                                        xorNode = &simplify(*xorLeftNode ^ *xorRighNode);
                                    }
                                    newGrandChildren = new std::vector<Node *>{factor, xorNode};
                                    break;
                                }
                                l += 1;
                            }
                            if (hasChanged) {
                                break;
                            }
                            k += 1;
                        }
                    }
                    else {
                        // case  GM(a,  b) ^ a -> GM(a, (b ^ 1)) / GM(a, b, c) ^ a -> GM(a, GM(b * c) ^ 1)
                        // case  a * b  + a    -> a * (b + 1)
                        // case  a & b  | a    -> a              / a & b & c | a -> a
                        // case (a | b) & a    -> a
                        // add  (a & b) ^ a    -> a & ~b ?
                        int32_t k = 0;
                        while (k < (int32_t) firstMul->children->size()) {
                            if (equivalence(*firstMul->children->at(k), *newChildren[j])) {
                                if (mulOp == IMUL || mulOp == GMUL) {
                                    Node * factor = newChildren[j];
                                    // Determining the xor operands
                                    std::vector<Node *> xorLeftChildren = *firstMul->children;
                                    xorLeftChildren.erase(xorLeftChildren.begin() + k);
                                    Node * xorLeftNode;
                                    if (xorLeftChildren.size() == 1) {
                                        xorLeftNode = xorLeftChildren[0];
                                    }
                                    else {
                                        xorLeftNode = &Node::OpNode(mulOp, xorLeftChildren);
                                    }

                                    Node * xorNode = NULL;
                                    if (mulOp == GMUL) {
                                        xorNode = &simplify(*xorLeftNode ^ Const(0x1, width));
                                    }
                                    else if (mulOp == IMUL) {
                                        xorNode = &simplify(*xorLeftNode + Const(0x1, width));
                                    }
                                    hasChanged = true;
                                    newGrandChildren = new std::vector<Node *>{factor, xorNode};
                                }
                                else if (mulOp == BAND && addOp == BXOR) {
                                    ;
                                }
                                else {
                                    hasChanged = true;
                                    newChildren.erase(newChildren.begin() + i);
                                    modified = true;
                                }
                                break;
                            }
                            k += 1;
                        }
                    }
                    if (hasChanged && newGrandChildren != NULL) {
                        newChildren.erase(newChildren.begin() + std::max(i, j));
                        newChildren.erase(newChildren.begin() + std::min(i, j));
                        Node * mulNode = &simplify(Node::OpNode(mulOp, *newGrandChildren));
                        delete newGrandChildren;
                        newChildren.push_back(mulNode);
                        modified = true;
                        break;
                    }
                    j += 1;
                }
            }
            if (hasChanged) {
                break;
            }
            i += 1;
        }
    }
    return modified;
}



Node & simplify(Node & node, bool propagateExtractInwards, bool useSingleBitVariables) {
    assert(not useSingleBitVariables or propagateExtractInwards);
    if (propagateExtractInwards and useSingleBitVariables) {
        return getBitDecomposition(node);
    }
    return simplifyCore(node, propagateExtractInwards, useSingleBitVariables);
}


Node & simplifyAndNotPEI(Node & node) {
    return simplifyCore(node, false, false);
}

Node & simplifyUSBV(Node & node) {
    return getBitDecomposition(node);
}

static Node & simplifyCore(Node & node, bool propagateExtractInwards, bool useSingleBitVariables) {
    assert(not useSingleBitVariables or propagateExtractInwards);

    auto setSimpEqAndReturn = [useSingleBitVariables](Node & node, Node & simpEquiv) -> Node & {
        Node & simpEq = getEquiv(simpEquiv);

        if (useSingleBitVariables) {
            node.simpEqUsbv = &simpEq;
            simpEq.simpEq = &simpEq;
            simpEq.simpEqUsbv = &simpEq;
        }
        else {
            node.simpEq = &simpEq;
            simpEq.simpEq = &simpEq;
        }
        //std::cout << "# Returning " << simpEq << std::endl;
        return simpEq;
    };

    if (node.nature == CONST || node.nature == STR) {
        return node;
    }

    if (useSingleBitVariables) {
        if (node.simpEqUsbv != NULL) {
            return *node.simpEqUsbv;
        }
    }
    else {
        if (node.simpEq != NULL) {
            return *node.simpEq;
        }
    }

    if (node.nature == SYMB) {
        if (useSingleBitVariables) {
            Node & s = getBitDecompositionVar(node);
            return setSimpEqAndReturn(node, s);
        }
        else {
            return setSimpEqAndReturn(node, node);
        }
    }

    //std::cout << "# Simplifying node " << node << std::endl;

    // After the "pre" recursive call part, the node to simplify is
    // defined with variable 'op' and list of children 'newChildren'
    std::vector<Node *> newChildren0;
    NodeOp op = node.op;
    int32_t width = node.width;

    if (op == EXTRACT) {
        Node * child = node.children->at(2);
        Node * msbNode = node.children->at(0);
        Node * lsbNode = node.children->at(1);
        //std::cout << "# Child of Extract to propagate inwards: " << child << std::endl;
        
        // At the end of this while loop, the current node (defined by op and newChildren0)
        // must not be an Extract node if propagateExtractInwards is True
        while (true) {
            assert(op == EXTRACT);
            //   child is the expression from which the extract occurs.
            //   For 2nd iteration onwards, it must be newChildren0[2]
            //   msbNode and lsbNode must also have the correct value
            // We do not look at child, msbNode and lsbNode in newChildren0
            // since it is not defined in 1st iteration and is used to detect a change later
            int32_t msb = (int32_t) msbNode->cst[0];
            int32_t lsb = (int32_t) lsbNode->cst[0];
            
            // QM FIXME: removed 'op == EXTRACT' here, propagate in verif_msi
            if (lsb == 0 && msb == child->width - 1) {
                if (child->nature == CONST) {
                    return setSimpEqAndReturn(node, *child);
                }
                if (child->nature == SYMB) {
                    if (useSingleBitVariables) {
                        Node & s = getBitDecompositionVar(*child);
                        return setSimpEqAndReturn(node, s);
                    }
                    else {
                        return setSimpEqAndReturn(node, *child);
                    }
                }
                op = child->op;
                newChildren0 = *child->children;
                if (op == EXTRACT) {
                    child = newChildren0[2];
                    msbNode = newChildren0[0];
                    lsbNode = newChildren0[1];
                    msb = msbNode->cst[0];
                    lsb = lsbNode->cst[0];
                    continue;
                }
                else {
                    break;
                }
            }
        
            if (child->nature == SYMB) {
                if (useSingleBitVariables) {
                    Node & s = getBitDecompositionVar(*child, msb, lsb);
                    return setSimpEqAndReturn(node, s);
                }
                else {
                    if (newChildren0.size() == 0) {
                        return setSimpEqAndReturn(node, node);
                    }
                    else {
                        return setSimpEqAndReturn(node, Extract(*newChildren0[0], *newChildren0[1], *newChildren0[2]));
                    }
                }
            }
            
            else if (child->nature == CONST) {
                return setSimpEqAndReturn(node, ConstNodeFromExtract(msb, lsb, *child));
            }
            
            assert(child->nature == OP);
            if (child->op == EXTRACT) {
                // Extract(m, l, Extract(v, u, e)) -> Extract(m + u, l + u, e)
                // Case is possible even if SimplifyExtract has been called (three or more nested Extract)
                Node & gLsbNode = *child->children->at(1);
                int32_t gLsb = (int32_t) gLsbNode.cst[0];
                msbNode = &Node::ConstNodeAuto(msb + gLsb);
                lsbNode = &Node::ConstNodeAuto(lsb + gLsb);
                child = child->children->at(2);
                newChildren0 = {msbNode, lsbNode, child};
                op = EXTRACT;
                continue; 
            }
            else if (child->op == CONCAT) {
                Node & concatNode = *child;
                int32_t startBitIdx = lsb;
                int32_t nbBitsOnRight = 0;
                int32_t i = 0;
                while (concatNode.children->at(i)->width <= startBitIdx) {
                    int32_t concatChildWidth = concatNode.children->at(i)->width;
                    startBitIdx -= concatChildWidth;
                    nbBitsOnRight += concatChildWidth;
                    i += 1;
                }

                int32_t startChildIdx = i;

                while (nbBitsOnRight + concatNode.children->at(i)->width <= msb) {
                    nbBitsOnRight += concatNode.children->at(i)->width;
                    i += 1;
                }
                int32_t endChildIdx = i;
                int32_t endBitIdx = msb - nbBitsOnRight;
                assert(endChildIdx < (int32_t) concatNode.children->size());

                if (startChildIdx == endChildIdx) {
                    // Extract contained in a single element (resNode), we remove Concat in all cases
                    Node & resNode = *concatNode.children->at(startChildIdx);
                    if (startBitIdx == 0 && endBitIdx == resNode.width - 1) {
                        // Also remove Extract
                        if (resNode.nature == CONST) {
                            return setSimpEqAndReturn(node, resNode);
                        }
                        if (resNode.nature == SYMB) {
                            if (useSingleBitVariables) {
                                Node & s = getBitDecompositionVar(resNode);
                                return setSimpEqAndReturn(node, s);
                            }
                            else {
                                return setSimpEqAndReturn(node, resNode);
                            }
                        }
                        newChildren0 = *resNode.children;
                        op = resNode.op;
                        if (op == EXTRACT) {
                            msbNode = newChildren0[0];
                            lsbNode = newChildren0[1];
                            child = newChildren0[2];
                            continue;
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        // Remove concat only, create new Extract with updated bounds and loop
                        msbNode = &Node::ConstNodeAuto(endBitIdx);
                        lsbNode = &Node::ConstNodeAuto(startBitIdx);
                        child = &resNode;
                        newChildren0 = {msbNode, lsbNode, &resNode};
                        op = EXTRACT;
                        continue;
                    }
                }

                // Extract covers 2 children or more
                Node & lsConcatChild = *concatNode.children->at(startChildIdx); // Least Significant child, on the right, at index 0 in list of concat children
                Node * lsNode;
                if (startBitIdx == 0) {
                    lsNode = &lsConcatChild;
                }
                else {
                    lsNode = &Extract(lsConcatChild.width - 1, startBitIdx, lsConcatChild);
                }
                Node & msConcatChild = *concatNode.children->at(endChildIdx);
                Node * msNode;
                if (endBitIdx == msConcatChild.width - 1) {
                    msNode = &msConcatChild;
                }
                else {
                    msNode = &Extract(endBitIdx, 0, msConcatChild);
                }
                newChildren0.clear();
                newChildren0.push_back(lsNode);
                newChildren0.insert(newChildren0.end(), concatNode.children->begin() + startChildIdx + 1, concatNode.children->begin() + endChildIdx);
                newChildren0.push_back(msNode);
                op = CONCAT;
                break;
            }

            else if (child->op == ZEXT || child->op == SEXT) {
                assert(false);
            }

            else if (child->op == LSHL) {
                Node & gchild = *child->children->at(0);
                Node & shNode = *child->children->at(1);
                int32_t sh = (int32_t) shNode.cst[0];
                if (msb < sh) {
                    return setSimpEqAndReturn(node, Const(0, msb - lsb + 1));
                }
                else {
                    if (lsb >= sh) {
                        // Extract(m, l, e << sh) -> Extract(m - sh, l - sh, e)
                        child = &gchild;
                        msbNode = &Node::ConstNodeAuto(msb - sh);
                        lsbNode = &Node::ConstNodeAuto(lsb - sh);
                        newChildren0 = {msbNode, lsbNode, child};
                        op = EXTRACT;
                        continue;
                    }
                    else {
                        // We have lsb - sh < 0
                        // Extract(m, l, e << sh) -> Concat(Extract(m - sh, 0, e), 0 [sh - l])
                        Node & newExtractNode = Extract(msb - sh, 0, gchild);
                        Node & zeroNode = Const(0, sh - lsb);
                        newChildren0 = {&zeroNode, &newExtractNode};
                        op = CONCAT;
                        break;
                    }
                }
            }

            else if (child->op == LSHR) {
                Node & gchild = *child->children->at(0);
                Node & shNode = *child->children->at(1);
                int32_t sh = (int32_t) shNode.cst[0];
                if (lsb >= gchild.width - sh) {
                    return setSimpEqAndReturn(node, Const(0, msb - lsb + 1));
                }
                else {
                    if (msb < gchild.width - sh) {
                        // Extract(m, l, LShR(e, sh)) -> Extract(m + sh, l + sh, e)
                        child = &gchild;
                        msbNode = &Node::ConstNodeAuto(msb + sh);
                        lsbNode = &Node::ConstNodeAuto(lsb + sh);
                        newChildren0 = {msbNode, lsbNode, child};
                        op = EXTRACT;
                        continue;
                    }
                    else {
                        // Extract(m, l, LShR(e, sh)) -> Concat(0  [m - (e.w - sh) + 1], Extract(e.w - 1, lsb + sh, e))
                        msbNode = &Node::ConstNodeAuto(gchild.width - 1);
                        lsbNode = &Node::ConstNodeAuto(lsb + sh);
                        Node & newExtractNode = Extract(*msbNode, *lsbNode, gchild);
                        Node & zeroNode = Const(0, msb - (gchild.width - sh) + 1);
                        newChildren0 = {&newExtractNode, &zeroNode};
                        op = CONCAT;
                        break;
                    }
                }
            }
 
            else if (child->op == ASHR) {
                Node & gchild = *child->children->at(0);
                Node & shNode = *child->children->at(1);
                int32_t sh = (int32_t) shNode.cst[0];
                Node & gchildMsb = Extract(gchild.width - 1, gchild.width - 1, gchild);
                if (lsb >= gchild.width - sh - 1) {
                    if (msb == lsb) {
                        // We avoid the Concat of a single node (gchildMsb)
                        // gchildMsb is reanalyzed
                        child = &gchild;
                        msbNode = &Node::ConstNodeAuto(gchild.width - 1);
                        lsbNode = msbNode;
                        newChildren0 = {msbNode, lsbNode, child};
                        op = EXTRACT;
                        continue;
                    }
                    else {
                        newChildren0.assign(msb - lsb + 1, &gchildMsb);
                        op = CONCAT;
                        break;
                    }
                }
                else {
                    if (msb < gchild.width - sh) {
                        // Extract(m, l, e >> sh) -> Extract(m + sh, l + sh, e)
                        child = &gchild;
                        msbNode = &Node::ConstNodeAuto(msb + sh);
                        lsbNode = &Node::ConstNodeAuto(lsb + sh);
                        newChildren0 = {msbNode, lsbNode, child};
                        op = EXTRACT;
                        continue;
                    }
                    else {
                        // Extract(m, l, e >> sh) -> Concat(e[w - 1] [m - (e.w - sh) + 1], Extract(e.w - 1, lsb + sh, e))
                        msbNode = &Node::ConstNodeAuto(gchild.width - 1);
                        lsbNode = &Node::ConstNodeAuto(lsb + sh);
                        Node & newExtractNode = Extract(*msbNode, *lsbNode, gchild);
                        newChildren0.assign(1, &newExtractNode);
                        newChildren0.insert(newChildren0.end(), msb - (gchild.width - sh) + 1, &gchildMsb);
                        op = CONCAT;
                        break;
                    }
                }
            }

            else if (propagateExtractInwards && Node::bitwiseOps.contains(child->op)) {
                // Extract(m, l, e & f) -> Extract(m, l, e) & Extract(m, l, f)
                newChildren0.clear();
                for (const auto & gchild : *child->children) {
                    Node & newExtractNode = Extract(*msbNode, *lsbNode, *gchild);
                    newChildren0.push_back(&newExtractNode);
                }
                op = child->op;
                break;
            }

            else if (propagateExtractInwards && child->op == BNOT) {
                Node & gchild = *child->children->at(0);
                Node & newExtractNode = Extract(*msbNode, *lsbNode, gchild);
                newChildren0 = {&newExtractNode};
                op = BNOT;
                break;
            }

            #if BIT_SIMPLIFY_PLUS
            else if (propagateExtractInwards && child->op == PLUS) {
                if (msb == lsb) {
                    int32_t idx = 0;
                    Node & child0 = *child->children->at(0);
                    std::vector<Node *> child0Bits;
                    for (int32_t b = 0; b < msb + 1; b += 1) {
                        child0Bits.push_back(&Extract(b, b, child0));
                    }
                    Node * ci = NULL;
                    Node * aiXorbi = NULL;
                    while (idx < (int32_t) child->children->size() - 1) {
                        std::vector<Node *> res;
                        ci = &Const(0, 1);
                        Node & child1 = *child->children->at(idx + 1);
                        for (int32_t b = 0; b < msb + 1; b += 1) {
                            Node & ai = *child0Bits[b];
                            Node & bi = Extract(b, b, child1);
                            aiXorbi = &(ai ^ bi);
                            if (b != msb || idx != (int32_t) child->children->size() - 2) {
                                Node & si = *aiXorbi ^ *ci;
                                res.push_back(&si);
                            }
                            if (b != msb) {
                                ci = &((*aiXorbi & *ci) | (ai & bi));
                            }
                        }
                        child0Bits = res;
                        idx += 1;
                    }
                    newChildren0 = {aiXorbi, ci};
                    op = BXOR;
                }
                else {
                    int32_t idx = 0;
                    Node & child0 = *child->children->at(0);
                    std::vector<Node *> child0Bits;
                    for (int32_t b = 0; b < msb + 1; b += 1) {
                        child0Bits.push_back(&Extract(b, b, child0));
                    }
                    std::vector<Node *> res;
                    while (idx < (int32_t) child->children->size() - 1) {
                        res.clear();
                        Node * ci = &Const(0, 1);
                        Node & child1 = *child->children->at(idx + 1);
                        for (int32_t b = 0; b < msb + 1; b += 1) {
                            Node & ai = *child0Bits[b];
                            Node & bi = Extract(b, b, child1);
                            Node & aiXorbi = ai ^ bi;
                            Node & si = aiXorbi ^ *ci;
                            res.push_back(&si);
                            if (b != msb) {
                                ci = &((aiXorbi & *ci) | (ai & bi));
                            }
                        }
                        child0Bits = res;
                        idx += 1;
                    }
                    newChildren0.clear();
                    newChildren0.insert(newChildren0.end(), res.begin() + lsb, res.begin() + msb + 1);
                    op = CONCAT;
                }
                break;
            }

            else if (propagateExtractInwards && child->op == UMINUS) {
                if (msb == lsb) {
                    Node * ci = &Const(1, 1);
                    Node * ai = NULL;
                    Node & child0 = *child->children->at(0);
                    for (int32_t b = 0; b < msb + 1; b += 1) {
                        ai = &(~Extract(b, b, child0));
                        if (b != msb) {
                            ci = &(*ai & *ci);
                        }
                    }
                    newChildren0 = {ai, ci};
                    op = BXOR;
                    break;
                }
                else {
                    std::vector<Node *> res;
                    Node * ci = &Const(1, 1);
                    Node & child0 = *child->children->at(0);
                    for (int32_t b = 0; b < msb + 1; b += 1) {
                        Node & ai = ~Extract(b, b, child0);
                        Node & si = ai ^ *ci;
                        res.push_back(&si);
                        if (b != msb) {
                            ci = &(ai & *ci);
                        }
                    }
                    newChildren0.clear();
                    newChildren0.insert(newChildren0.end(), res.begin() + lsb, res.begin() + msb + 1);
                    op = CONCAT;
                    break;
                }
            }
            #endif

            else if (propagateExtractInwards && (child->op == GEXP || child->op == GLOG) && msb == 7 && lsb == 0) {
                // Extract(7, 0, GExp(e)) -> GExp(Extract(7, 0, e))
                // Condition could be more precise / different: child.children[0]).op == 'C', len(child.children[0]) == 2, child.children[0].children[0].cst == 0, child.width > 8, etc.
                // This is to deal with the case Extract(m, l, GExp(Concat(0, e))) -> GExp(e)
                Node & newExtractNode = Extract(msb, lsb, *child->children->at(0));
                newChildren0 = {&newExtractNode};
                op = child->op;
                break;
            }

            else {
                //std::cout << "node: %s" << node << std::endl;
                break;
            }
        } // while (true)
    } // op == EXTRACT


    /////////////////////////
    // Main recursion loop //
    /////////////////////////

    bool modified = (newChildren0.size() != 0);
    std::vector<Node *> newChildren;
    if (not modified) {
        std::vector<Node *> & oldChildren = *node.children;

        if (useSingleBitVariables and Node::wordOps.contains(op)) {
            for (const auto & child : *node.children) {
                newChildren.push_back(&getBitDecomposition(*child));
            }
        }
        else {
            for (const auto & child : *node.children) {
                newChildren.push_back(&simplifyCore(*child, propagateExtractInwards, useSingleBitVariables));
            }
        }

        for (int32_t i = 0; i < (int32_t) oldChildren.size(); i += 1) {
            if (newChildren[i] != oldChildren[i]) {
                modified = true;
                break;
            }
        }
    }
    else {
        if (useSingleBitVariables and Node::wordOps.contains(op)) {
            for (const auto & child : newChildren0) {
                newChildren.push_back(&getBitDecomposition(*child));
            }
        }
        else {
            for (const auto & child : newChildren0) {
                newChildren.push_back(&simplifyCore(*child, propagateExtractInwards, useSingleBitVariables));
            }
        }
    }


    // Preprocessing before main switch
    // These operations must be done before merging concat chilren
    bool addNotNode = false;
    NodeOp op1 = NOP;
    Node * cst0 = NULL;
    Node * cst1 = NULL;
    if (op == BXOR) {
        addNotNode = false;
        // Necessary to cut into two while loops: the exp k0 ^ ~k0 will not be simplified in one pass
        // (the ~ would be removed when processing the second child and then the comparison with the first child would not be done)
        int32_t i = 0;
        while (i < (int32_t) newChildren.size()) {
            Node & child0 = *newChildren[i];
            if (child0.op == BNOT) {
                addNotNode = !addNotNode;
                modified = true;
                // Removing not node in child
                newChildren[i] = newChildren[i]->children->at(0);
            }
            i += 1;
        }
    }
    else if (op == BAND) {
        //print('simp & [ ' + ', '.join(map(lambda x: '%s' % x, newChildren)) + ' ]')
        op1 = BOR;
        cst0 = &Const(0, width);
        cst1 = &Const(-1, width, true);
    }
    else if (op == BOR) {
        //print('simp | [ ' + ', '.join(map(lambda x: '%s' % x, newChildren)) + ' ]')
        op1 = BAND;
        cst0 = &Const(-1, width, true);
        cst1 = &Const(0, width);
    }

    // Calling mergeWithChildrenIfPossible and mergeConcatChildren
    // Done here because for example for '^', removing '~' node can make new '^' nodes as children
    // (Note those '^' nodes should not have any '~' children)
    // FIXME: propagate bugfix in verif_msi
    if (newChildren.size() > 0 && (op == BXOR || op == BAND || op == BOR || op == PLUS)) {
        bool m = mergeWithChildrenIfPossible(op, newChildren);
        modified = modified || m;

        if (op != PLUS) {
            m = mergeConcatChildren(op, newChildren, &op, useSingleBitVariables);
            modified = modified || m;
        }
    }


    if (op == BXOR) {
        bool modifiedByFactorization;
        do {
            modifiedByFactorization = false;
            // simplify e ^ 0 to e
            // simplify e ^ e to 0
            // simplify e ^ 1 to ~e
            // simplify ~a ^ ~b ^ ~c to ~(a ^ b ^ c)
            // Note: it is possible to avoid the "erase" operation by allocating a new vector
            //       of non-deleted elements and copying it to newChildren at the end if required
            //       However, I think the cost of the copy alone does compare with the erase cost
            //       not to mention the allocation and deletion of a local vector, for usually small
            //       sizes of newChildren   
            {
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node & child0 = *newChildren[i];
                    int32_t j = i + 1;
                    bool removed = false;
                    while (j < (int32_t) newChildren.size()) {
                        Node & child1 = *newChildren[j];
                        if (equivalence(child0, child1)) {
                            // Suppressing child0 and child1 from list starting
                            // with child1 in order not to modify child0 index
                            newChildren.erase(newChildren.begin() + j);
                            newChildren.erase(newChildren.begin() + i);
                            removed = true;
                            modified = true;
                            break;
                        }
                        j += 1;
                    }
                    if (!removed) {
                        i += 1;
                    }
                }
            }

            // Constant propagation
            // The node is modified if:
            // - Two or more children are constant
            // OR
            // - One children is a constant and it is equal to 0 or -1
            {
                int32_t i = 0;
                int32_t nlimbs = width / 64;
                if (nlimbs * 64 != width) {
                    nlimbs += 1;
                }
                uint64_t constVal[nlimbs];
                std::fill_n(constVal, nlimbs, 0);
                int32_t previousConstIdx = -1;
                while (i < (int32_t) newChildren.size()) {
                    Node & child = *newChildren[i];
                    if (child.nature == CONST) {
                        if (previousConstIdx == -2) {
                            // Two or more previous constants
                            for (int32_t j = 0; j < nlimbs; j += 1) {
                                constVal[j] = constVal[j] ^ child.cst[j];
                            }
                            newChildren.erase(newChildren.begin() + i);
                            continue;
                        }
                        else if (previousConstIdx != -1) {
                            // 2nd encountered constant
                            // We remove the previous constant
                            for (int32_t j = 0; j < nlimbs; j += 1) {
                                constVal[j] = newChildren[previousConstIdx]->cst[j] ^ child.cst[j];
                            }
                            newChildren.erase(newChildren.begin() + previousConstIdx);
                            i -= 1;
                            newChildren.erase(newChildren.begin() + i);
                            modified = true;
                            previousConstIdx = -2;
                            continue;
                        }
                        else if (equivalence(child, Const(0, width))) {
                            newChildren.erase(newChildren.begin() + i);
                            modified = true;
                            continue;
                        }
                        else if (equivalence(child, Const(-1, width, true))) {
                            for (int32_t j = 0; j < nlimbs; j += 1) {
                                constVal[j] = child.cst[j];
                            }
                            newChildren.erase(newChildren.begin() + i);
                            modified = true;
                            previousConstIdx = -2;
                            continue;
                        }
                        else {
                            previousConstIdx = i;
                        }

                    }
                    i += 1;
                }
                if (previousConstIdx == -2) {
                    // Two or more constants, add node with value constVal if not 0
                    if (!isZero(constVal, width)) {
                        if (isAllOne(constVal, width)) {
                            addNotNode = !addNotNode;
                        }
                        else {
                            if (addNotNode) {
                                // Propagate not node on constant
                                for (int32_t j = 0; j < nlimbs; j += 1) {
                                    constVal[j] ^= 0xFFFFFFFFFFFFFFFFULL;
                                }
                                if (width % 64 != 0) {
                                    constVal[nlimbs - 1] &= (1ULL << (width % 64)) - 1;
                                }
                                addNotNode = false;
                            }
                            newChildren.push_back(&Const(constVal, nlimbs, width));
                        }
                    }
                }
            }


            // a * b ^ a * c -> a * (b ^ c)
            // Remark: the factorisation is not unique
            //   a * b ^ a * c ^ b * c
            // = a * (b ^ c) ^ b * c
            // = b * (a ^ c) ^ a * c
            // = c * (a ^ b) ^ a * b
            if (node.hasWordOp) {
                bool m = factorize(GMUL, BXOR, newChildren, width);
                modified = modified || m;
                modifiedByFactorization = modifiedByFactorization || m;
            }

            //std::cout << "children at 0: [ ";
            //for (const auto & nc : newChildren) {
            //    std::cout << nc->verbatimPrint() << ", ";
            //}
            //std::cout << "]" << std::endl;
            bool m = factorize(BAND, BXOR, newChildren, width);
            modified = modified || m;
            modifiedByFactorization = modifiedByFactorization || m;
            // NOTE: do-while loop here because it is possible here to have a const(0) among newChildren, or identical children, after factorization
        } while (modifiedByFactorization);


        // GPow(a, n) ^ GPow(b, n) and n is a power of 2 -> GPow(a ^ b, n)
        if (node.hasWordOp) {
            std::map<Node *, int32_t> nbPowChildren;
            for (int32_t i = 0; i < (int32_t) newChildren.size(); i += 1){
                Node & child = *newChildren[i];
                if (child.op == GPOW) {
                    Node * gchild = child.children->at(1);
                    if (gchild->nature == CONST && Node::powersTwo.contains(gchild->cst[0])) {
                        if (nbPowChildren.contains(gchild)) {
                            nbPowChildren[gchild] += 1;
                        }
                        else {
                            nbPowChildren[gchild] = 1;
                        }
                    }
                }
            }


            for (const auto & [exp, val] : nbPowChildren) {
                if (val > 1) {
                    modified = true;
                    std::vector<Node *> powChildren;
                    int32_t i = 0;
                    while (i < (int32_t) newChildren.size()) {
                        Node & child = *newChildren[i];
                        if (child.op == GPOW && exp == child.children->at(1)) {
                            powChildren.push_back(child.children->at(0));
                            newChildren.erase(newChildren.begin() + i);
                            continue;
                        }
                        i += 1;
                    }
                    // QM FIXME: what if usbv is used?
                    Node & newAddNode = simplify(Node::OpNode(BXOR, powChildren));
                    Node & newPowNode = simplify(GPow(newAddNode, *exp));
                    newChildren.push_back(&newPowNode);
                }
            }
        }


        // Final considerations
        if (newChildren.size() == 0) {
            if (addNotNode) {
                return setSimpEqAndReturn(node, Const(-1, width, true));
            }
            else {
                return setSimpEqAndReturn(node, Const(0, width));
            }
        }
        else if (newChildren.size() == 1) {
            if (addNotNode) {
                return setSimpEqAndReturn(node,Node:: OpNode(BNOT, newChildren));
            }
            else {
                return setSimpEqAndReturn(node, *newChildren[0]);
            }
        }
        else {
            if (modified) {
                Node & xorNode = Node::OpNode(BXOR, newChildren);
                if (addNotNode) {
                    return setSimpEqAndReturn(node, Node::OpNode(BNOT, {&xorNode}));
                }
                else {
                    return setSimpEqAndReturn(node, xorNode);
                }
            }
            else {
                return setSimpEqAndReturn(node, node);
            }
        }
    }


    else if (op == PLUS) {
        // Constant propagation
        {
            int32_t i = 0;
            int32_t nlimbs = width / 64;
            if (nlimbs * 64 != width) {
                nlimbs += 1;
            }
            uint64_t res[nlimbs];
            std::fill_n(res, nlimbs, 0);
            while (i < (int32_t) newChildren.size()) {
                Node & child = *newChildren[i];
                if (child.nature == CONST) {
                    int32_t carry = 0;
                    for (int32_t j = 0; j < nlimbs - 1; j += 1) {
                        res[j] += child.cst[j] + carry;
                        carry = res[j] < child.cst[j] ? 1 : 0;
                    }
                    if (width % 64 == 0) {
                        res[nlimbs - 1] += child.cst[nlimbs - 1] + carry;
                    }
                    else {
                        res[nlimbs - 1] = (res[nlimbs - 1] + child.cst[nlimbs - 1] + carry) & ((1ULL << (width % 64)) - 1);

                    }
                    newChildren.erase(newChildren.begin() + i);
                    modified = true;
                    continue;
                }
                i += 1;
            }

            if (!isZero(res, width)) {
                newChildren.push_back(&Const(res, nlimbs, width));
            }
        }

        // 2nd pass: equivalence: removing opposite expressions
        {
            int32_t i = 0;
            while (i < (int32_t) newChildren.size()) {
                Node & child0 = *newChildren[i];
                int32_t j = i + 1;
                bool removed = false;
                while (j < (int32_t) newChildren.size()) {
                    Node & child1 = *newChildren[j];
                    if ((child0.op == UMINUS && equivalence(*child0.children->at(0), child1)) || (child1.op == UMINUS && equivalence(child0, *child1.children->at(0)))) {
                        // Suppressing child0 and child1 starting from child1
                        newChildren.erase(newChildren.begin() + j);
                        newChildren.erase(newChildren.begin() + i);
                        removed = true;
                        modified = true;
                        break;
                    }
                    j += 1;
                }
                if (!removed) {
                    i += 1;
                }
            }
        }

        // Factorization
        if (node.hasWordOp) {
            bool m = factorize(IMUL, PLUS, newChildren, width);
            modified = modified || m;

            // GLog(x) + GLog(y) -> GLog(GMul(x, y))
            int32_t numLogChildren = 0;
            for (int32_t i = 0; i < (int32_t) newChildren.size(); i += 1) {
                if (newChildren[i]->op == GLOG) {
                    numLogChildren += 1;
                }
            }

            if (numLogChildren > 1) {
                modified = true;
                std::vector<Node *> logChildren;
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node & child = *newChildren[i];
                    if (child.op == GLOG) {
                        logChildren.push_back(child.children->at(0));
                        newChildren.erase(newChildren.begin() + i);
                        continue;
                    }
                    i += 1;
                }
                // QM FIXME: what if usbv is used?
                Node & mulNode = simplify(Node::OpNode(GMUL, logChildren));
                Node & logNode = simplify(GLog(mulNode));
                newChildren.push_back(&logNode);
            }
        }


        if (newChildren.size() == 0) {
            return setSimpEqAndReturn(node, Const(0, width));
        }
        if (newChildren.size() == 1) {
            return setSimpEqAndReturn(node, *newChildren[0]);
        }
        else {
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }

 
    else if (op == UMINUS) {
        // -(-e) -> e
        Node & child = *newChildren[0];
        if (child.op == UMINUS) {
            return setSimpEqAndReturn(node, *child.children->at(0));
        }
        else if (child.nature == CONST) {
            int32_t nlimbs = child.nlimbs;
            uint64_t res[nlimbs];
            int32_t carry = 1;
            for (int32_t i = 0; i < nlimbs; i += 1) {
                res[i] = ~child.cst[i] + carry;
                carry = res[i] < ~child.cst[i] ? 1 : 0;
            }
            if (width % 64 != 0) {
                res[nlimbs - 1] = res[nlimbs - 1] % (1ULL << (width % 64));
            }
            return setSimpEqAndReturn(node, Const(res, nlimbs, width));

        }
        else {
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }


    else if (op == BAND || op == BOR) {
        bool hasChanged = true;
        while (hasChanged) {
            hasChanged = false;
            {
                // simplify e & e and e | e to e
                // Not setting hasChanged to true since it is the first rule:
                // we avoid making an additional round of simp rules if it is the only rule taken
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node & child0 = *newChildren[i];
                    int32_t j = i + 1;
                    while (j < (int32_t) newChildren.size()) {
                        Node & child1 = *newChildren[j];
                        if (equivalence(child0, child1)) {
                            // Suppressing child1
                            newChildren.erase(newChildren.begin() + j);
                            modified = true;
                        }
                        else {
                            j += 1;
                        }
                    }
                    i += 1;
                }
            }
            {
                // simplify e & ~e to 0 and e | ~e to 1
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node & child0 = *newChildren[i];
                    int32_t j = i + 1;
                    while (j < (int32_t) newChildren.size()) {
                        Node & child1 = *newChildren[j];
                        if ((child0.op == BNOT && equivalence(*child0.children->at(0), child1)) || (child1.op == BNOT && equivalence(child0, *child1.children->at(0)))) {
                            return setSimpEqAndReturn(node, *cst0);
                        }
                        else {
                            j += 1;
                        }
                    }
                    i += 1;
                }
            }
            {
                // FIXME: propagate in VerifMSI
                // Simplify  a &  (a ^ b) to  a & ~b
                //          ~a &  (a ^ b) to ~a &  b
                //           a & ~(a ^ b) to  a &  b
                //          ~a & ~(a ^ b) to ~a & ~b
                //           a |  (a ^ b) to  a |  b
                //          ~a |  (a ^ b) to ~a | ~b
                //           a | ~(a ^ b) to  a | ~b
                //          ~a | ~(a ^ b) to ~a |  b
                // Note: the case a & (~a ^ b) cannot appear since the xor not has been simplified and the '~' would be moved above the xor
                // These rules are included in the while loop since a factorisation can make new opportunities for simplification
#if EXTENDED_SIMPLIFY
                // Allows to take into account cases in which 'a' is a BXOR node in the examples above
                // Example: (a ^ b) & (a ^ b ^ c) -> a ^ b & ~c
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node * child0 = newChildren[i];
                    if (child0->op == BXOR or (child0->op == BNOT and child0->children->at(0)->op == BXOR)) {
                        bool xorHasNot = (op == BOR);
                        if (child0->op == BNOT) {
                            child0 = child0->children->at(0);
                            xorHasNot = !xorHasNot;
                        }
                        int32_t k = 0;
                        std::set<int32_t> idxToRemove;
                        std::set<int32_t> tempIdxToRemove;
                        while (k < (int32_t) newChildren.size()) {
                            if (i == k) {
                                k += 1;
                                continue;
                            }
                            Node * child1 = newChildren[k];
                            bool varHasNot = false;
                            if (child1->op == BNOT) {
                                child1 = child1->children->at(0);
                                varHasNot = true;
                            }
                            if (child1->op == BXOR) {
                                // We search for all the children of child1 in the children of child0
                                // If they are all present, we remove all these children from child0
                                bool allPresent = true;
                                tempIdxToRemove.clear();
                                for (const auto & gChild1 : *child1->children) {
                                    bool present = false;
                                    int32_t j = 0;
                                    while (j < (int32_t) child0->children->size()) {
                                        Node & xorChild = *child0->children->at(j);
                                        if (equivalence(*gChild1, xorChild)) {
                                            present = true;
                                            tempIdxToRemove.insert(j);
                                            break;
                                        }
                                        j += 1;
                                    }
                                    allPresent = allPresent and present;
                                    if (!allPresent) {
                                        break;
                                    }
                                }

                                if (allPresent) {
                                    // Copy the temporary indexes to remove in idxToRemove
                                    idxToRemove.insert(tempIdxToRemove.begin(), tempIdxToRemove.end());
                                    xorHasNot = xorHasNot ^ (!varHasNot);
                                }
                            }
                            else {
                                int32_t j = 0;
                                while (j < (int32_t) child0->children->size()) {
                                    Node & xorChild = *child0->children->at(j);
                                    if (equivalence(*child1, xorChild)) {
                                        idxToRemove.insert(j);
                                        xorHasNot = xorHasNot ^ (!varHasNot);
                                        break;
                                    }
                                    j += 1;
                                }
                            }
                            if (idxToRemove.size() == child0->children->size() - 1) {
                                break;
                            }
                            k += 1;
                        }

                        if (idxToRemove.size() != 0) {
                            std::vector<Node *> newGrandChildren;
                            for (int32_t idx = 0; idx < (int32_t) child0->children->size(); idx += 1) {
                                if (!idxToRemove.contains(idx)) {
                                    newGrandChildren.push_back(child0->children->at(idx));
                                }
                            }
                            // assert(newGrandChildren.size() != 0); // Case is possible
                            Node * newChild;
                            if (newGrandChildren.size() == 0) {
                                newChild = &Const(0, width);
                            }
                            else if (newGrandChildren.size() == 1) {
                                newChild = newGrandChildren[0];
                            }
                            else {
                                newChild = &Node::OpNode(BXOR, newGrandChildren);
                            }
                            if (xorHasNot) {
                                newChild = &~(*newChild);
                            }
                            newChildren[i] = newChild;
                            bool m = mergeWithChildrenIfPossible(op, newChildren);
                            (void) m;
                            hasChanged = true;
                            modified = true;
                        }
                    }
                    i += 1;
                }
#else
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node * child0 = newChildren[i];
                    if (child0->op == BXOR or (child0->op == BNOT and child0->children->at(0)->op == BXOR)) {
                        bool xorHasNot = (op == BOR);
                        if (child0->op == BNOT) {
                            child0 = child0->children->at(0);
                            xorHasNot = !xorHasNot;
                        }
                        int32_t j = 0;
                        std::set<int32_t> idxToRemove;
                        while (j < (int32_t) child0->children->size()) {
                            Node & xorChild = *child0->children->at(j);
                            int32_t k = 0;
                            while (k < (int32_t) newChildren.size()) {
                                if (i == k) {
                                    k += 1;
                                    continue;
                                }
                                Node * child1 = newChildren[k];
                                bool varHasNot = false;
                                if (child1->op == BNOT) {
                                    child1 = child1->children->at(0);
                                    varHasNot = true;
                                }
                                if (equivalence(*child1, xorChild)) {
                                    idxToRemove.insert(j);
                                    xorHasNot = xorHasNot ^ (!varHasNot);
                                    break;
                                } 
                                k += 1;
                            }
                            if (idxToRemove.size() == child0->children->size() - 1) {
                                break;
                            }
                            j += 1;
                        }
                        if (idxToRemove.size() != 0) {
                            std::vector<Node *> newGrandChildren;
                            for (int32_t idx = 0; idx < (int32_t) child0->children->size(); idx += 1) {
                                if (!idxToRemove.contains(idx)) {
                                    newGrandChildren.push_back(child0->children->at(idx));
                                }
                            }
                            assert(newGrandChildren.size() != 0);
                            Node * newChild;
                            if (newGrandChildren.size() == 1) {
                                newChild = newGrandChildren[0];
                            }
                            else {
                                newChild = &Node::OpNode(BXOR, newGrandChildren);
                            }
                            if (xorHasNot) {
                                newChild = &~(*newChild);
                            }
                            newChildren[i] = newChild;
                            bool m = mergeWithChildrenIfPossible(op, newChildren);
                            (void) m;
                            hasChanged = true;
                            modified = true;
                        }
                    }
                    i += 1;
                }
#endif
            }
            {
                // simplify a | (~a & b) to a | b
                //          a & (~a | b) to a & b
                // an iteration loop is necessary, until no change is made
                // e.g.                (~a & ~b & c) | (~a & b) | a
                // after 1 iteration:  (~b & c) | b | a
                // after 2 iterations: c | b | a
#if EXTENDED_SIMPLIFY
                // Deals with cases such as:
                //   a0 | ~a1 | a2  | ~(a0 | ~a1 | a2) & b ->   a0 | ~a1 | a2  | b
                // ~(a0 | ~a1 | a2) |  (a0 | ~a1 | a2) & b -> ~(a0 | ~a1 | a2) | b
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node & child0 = *newChildren[i];
                    if (child0.op == op1) {
                        int32_t j = 0;
                        std::set<int32_t> idxToRemove;
                        while (j < (int32_t) child0.children->size()) {
                            Node & op1Child = *child0.children->at(j);
                            if (op1Child.op == BNOT) {
                                Node & op1gChild = *op1Child.children->at(0);
                                if (op1gChild.op == op) {
                                    // We search for every op1gChild child in newChildren
                                    bool allPresent = true;
                                    for (const auto & op1ggChild : *op1gChild.children) {
                                        bool found = false;
                                        int32_t k = 0;
                                        while (k < (int32_t) newChildren.size()) {
                                            if (i == k) {
                                                k += 1;
                                                continue;
                                            }
                                            if (equivalence(*op1ggChild, *newChildren[k])) {
                                                found = true;
                                                break;
                                            }
                                            k += 1;
                                        }
                                        allPresent = allPresent && found;
                                        if (!allPresent) {
                                            break;
                                        }
                                    }
                                    if (allPresent) {
                                        idxToRemove.insert(j);
                                    }

                                }
                                else {
                                    // standard case
                                    int32_t k = 0;
                                    while (k < (int32_t) newChildren.size()) {
                                        if (i == k) {
                                            k += 1;
                                            continue;
                                        }
                                        Node & child1 = *newChildren[k];
                                        if (equivalence(child1, op1gChild)) {
                                            idxToRemove.insert(j);
                                            break;
                                        }
                                        k += 1;
                                    }
                                }
                            }
                            else {
                                // standard case
                                int32_t k = 0;
                                while (k < (int32_t) newChildren.size()) {
                                    if (i == k) {
                                        k += 1;
                                        continue;
                                    }
                                    Node & child1 = *newChildren[k];
                                    if (child1.op == BNOT && equivalence(*child1.children->at(0), op1Child)) {
                                        idxToRemove.insert(j);
                                        break;
                                    }
                                    k += 1;
                                }
                            }
                            j += 1;
                        }
                        if (idxToRemove.size() != 0) {
                            std::vector<Node *> newGrandChildren;
                            for (int32_t idx = 0; idx < (int32_t) child0.children->size(); idx += 1) {
                                if (!idxToRemove.contains(idx)) {
                                    newGrandChildren.push_back(child0.children->at(idx));
                                }
                            }
                            Node * newChild;
                            if (newGrandChildren.size() == 0) {
                                newChild = cst1;
                            }
                            else if (newGrandChildren.size() == 1) {
                                newChild = newGrandChildren[0];
                            }
                            else {
                                newChild = &Node::OpNode(op1, newGrandChildren);
                            }
                            newChildren[i] = newChild;
                            bool m = mergeWithChildrenIfPossible(op, newChildren);
                            (void) m;
                            hasChanged = true;
                            modified = true;
                        }
                    }
                    i += 1;
                }
#else
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node & child0 = *newChildren[i];
                    if (child0.op == op1) {
                        int32_t j = 0;
                        std::set<int32_t> idxToRemove;
                        while (j < (int32_t) child0.children->size()) {
                            Node & op1Child = *child0.children->at(j);
                            int32_t k = 0;
                            while (k < (int32_t) newChildren.size()) {
                                if (i == k) {
                                    k += 1;
                                    continue;
                                }
                                Node & child1 = *newChildren[k];
                                if ((child1.op == BNOT && equivalence(*child1.children->at(0), op1Child)) || (op1Child.op == BNOT && equivalence(child1, *op1Child.children->at(0)))) {
                                    idxToRemove.insert(j);
                                    break;
                                }
                                k += 1;
                            }
                            j += 1;
                        }
                        if (idxToRemove.size() != 0) {
                            std::vector<Node *> newGrandChildren;
                            for (int32_t idx = 0; idx < (int32_t) child0.children->size(); idx += 1) {
                                if (!idxToRemove.contains(idx)) {
                                    newGrandChildren.push_back(child0.children->at(idx));
                                }
                            }
                            Node * newChild;
                            if (newGrandChildren.size() == 0) {
                                newChild = cst1;
                            }
                            else if (newGrandChildren.size() == 1) {
                                newChild = newGrandChildren[0];
                            }
                            else {
                                newChild = &Node::OpNode(op1, newGrandChildren);
                            }
                            newChildren[i] = newChild;
                            bool m = mergeWithChildrenIfPossible(op, newChildren);
                            (void) m;
                            hasChanged = true;
                            modified = true;
                        }
                    }
                    i += 1;
                }
#endif
            }
        }

        // simplify e | 0 to e, e | 1 to 1
        // simplify e & 0 to 0, e & 1 to e
        // + Factorization
        {
            int32_t i = 0;
            int32_t nlimbs = width / 64;
            if (nlimbs * 64 != width) {
                nlimbs += 1;
            }
            uint64_t constVal[nlimbs];
            for (int32_t j = 0; j < nlimbs; j += 1) {
                constVal[j] = cst1->cst[j];
            }
            while (i < (int32_t) newChildren.size()) {
                Node & child = *newChildren[i];
                if (child.nature == CONST) {
                    if (op == BAND) {
                        for (int32_t j = 0; j < nlimbs; j += 1) {
                            constVal[j] &= child.cst[j];
                        }
                    }
                    else {
                        for (int32_t j = 0; j < nlimbs; j += 1) {
                            constVal[j] |= child.cst[j];
                        }
                    }
                    newChildren.erase(newChildren.begin() + i);
                    modified = true;
                    continue;
                }
                i += 1;
            }

            // Factorization
            bool m = factorize(op1, op, newChildren, width);
            modified = modified || m;

            if ((op == BAND && isZero(constVal, width)) || (op == BOR && isAllOne(constVal, width))) {
                return setSimpEqAndReturn(node, *cst0);
            }
            if (newChildren.size() == 0 || ((op == BAND && !isAllOne(constVal, width)) || (op == BOR && !isZero(constVal, width)))) {
                // modified already set to True in while loop
                newChildren.push_back(&Const(constVal, nlimbs, width));
            }
        }

        // Final considerations
        if (newChildren.size() == 0) {
            return setSimpEqAndReturn(node, Const(0, width));
        }
        if (newChildren.size() == 1) {
            // Suppressing '&' or '|' node
            return setSimpEqAndReturn(node, *newChildren[0]);
        }
        else {
#if EXTENDED_SIMPLIFY
            // De Morgan
            // (~a & ~b & ~c) -> ~(a | b | c)
            // (~a | ~b | ~c) -> ~(a & b & c)
            {
                bool allChildrenBnot = true;
                for (const auto & c : newChildren) {
                    if (c->op != BNOT) {
                        allChildrenBnot = false;
                        break;
                    }
                }
                if (allChildrenBnot) {
                    std::vector<Node *> newGrandChildren;
                    for (int32_t i = 0; i < (int32_t) newChildren.size(); i += 1) {
                        newGrandChildren.push_back(newChildren[i]->children->at(0));
                    }
                    // call to simplify here because changing BAND to BOR can create possibilities for merging with children
                    // Also, if the result of the OpNode call is a constant, we need to simplify the '~' in the constant
                    // QM FIXME: what if usbv is used?
                    Node & op1Node = simplify(~Node::OpNode(op1, newGrandChildren));
                    return setSimpEqAndReturn(node, op1Node);
                }
            }
#endif
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }


    else if (op == BNOT) {
        assert(newChildren.size() == 1);
        Node & child = *newChildren[0];
        if (child.op == BNOT) {
            return setSimpEqAndReturn(node, *child.children->at(0));
        }
        else if (child.nature == CONST) {
            int32_t nlimbs = child.nlimbs;
            uint64_t res[nlimbs];
            for (int32_t i = 0; i < nlimbs - 1; i += 1) {
                res[i] = ~child.cst[i];
            }
            if (width % 64 == 0) {
                res[nlimbs - 1] = ~child.cst[nlimbs - 1];
            }
            else {
                res[nlimbs - 1] = ((1ULL << (width % 64)) - 1) ^ child.cst[nlimbs - 1];
            }
            return setSimpEqAndReturn(node, Const(res, nlimbs, width));
        }
        else {
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }

    else if (op == LSHL) {
        Node & exp = *newChildren[0];
        Node & sh0 = *newChildren[1];
        if (isZero(sh0.cst, sh0.width)) {
            return setSimpEqAndReturn(node, *newChildren[0]);
        }
        else {
            if (exp.op == LSHL) {
                Node & gChild = *exp.children->at(0);
                Node & sh1 = *exp.children->at(1);
                return setSimpEqAndReturn(node, gChild << (sh0.cst[0] + sh1.cst[0]));
            }
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }


    else if (op == ASHR) {
        Node & exp = *newChildren[0];
        Node & sh0 = *newChildren[1];
        if (isZero(sh0.cst, sh0.width)) {
            return setSimpEqAndReturn(node, *newChildren[0]);
        }
        else {
            if (exp.op == ASHR) {
                Node & gChild = *exp.children->at(0);
                Node & sh1 = *exp.children->at(1);
                return setSimpEqAndReturn(node, gChild >> (sh0.cst[0] + sh1.cst[0]));
            }
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }


    else if (op == LSHR) {
        Node & exp = *newChildren[0];
        Node & sh0 = *newChildren[1];
        if (isZero(sh0.cst, sh0.width)) {
            return setSimpEqAndReturn(node, *newChildren[0]);
        }
        else {
            if (exp.op == LSHR) {
                Node & gChild = *exp.children->at(0);
                Node & sh1 = *exp.children->at(1);
                return setSimpEqAndReturn(node, LShR(gChild, sh0.cst[0] + sh1.cst[0]));
            }
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }


    else if (op == EXTRACT) {
        // FIXME: propagate in verif_msi
        if (modified) {
            Node & newNode = Extract(*newChildren[0], *newChildren[1], *newChildren[2]);
            if (newNode.nature == CONST) {
                return setSimpEqAndReturn(node, newNode);
            }
            else {
                // QM FIXME: what if usbv is used?
                return setSimpEqAndReturn(node, simplifyExtract(newNode));
            }
        }
        else {
            return setSimpEqAndReturn(node, simplifyExtract(node));
        }
    }


    else if (op == CONCAT) {
        // Merge adjacent constant children
        std::vector<Node *> newChildrenMerged;
        std::vector<Node *> currentConstantChildren;
        for (const auto & child : newChildren) {
            if (child->nature == CONST) {
                currentConstantChildren.push_back(child);
            }
            else {
                if (currentConstantChildren.size() > 0) {
                    if (currentConstantChildren.size() > 1) {
                        modified = true;
                        newChildrenMerged.push_back(&ConstNodeFromConcat(currentConstantChildren));
                    }
                    else {
                        newChildrenMerged.push_back(currentConstantChildren[0]);
                    }
                    currentConstantChildren.clear();
                }
                newChildrenMerged.push_back(child);
            }
        }
        // Case when last children of Concat are constants
        if (currentConstantChildren.size() > 0) {
            if (currentConstantChildren.size() > 1) {
                modified = true;
                newChildrenMerged.push_back(&ConstNodeFromConcat(currentConstantChildren));
            }
            else {
                newChildrenMerged.push_back(currentConstantChildren[0]);
            }
        }

        if (newChildrenMerged.size() == 1) {
            assert(newChildrenMerged[0]->nature == CONST);
            return setSimpEqAndReturn(node, *newChildrenMerged[0]);
        }

        newChildren = newChildrenMerged;

        // Concat(e, Concat(f, g))
        bool mergeConcat = false;
        for (const auto & child : newChildren) {
            if (child->op == CONCAT) {
                modified = true;
                mergeConcat = true;
                break;
            }
        }
        if (mergeConcat) {
            newChildrenMerged.clear();
            for (const auto & child : newChildren) {
                if (child->op == CONCAT) {
                    for (const auto & gChild : *child->children) {
                        newChildrenMerged.push_back(gChild);
                    }
                }
                else {
                    newChildrenMerged.push_back(child);
                }
            }
            newChildren = newChildrenMerged;
        }


        // Case all children are Extract nodes with corresponding bit indexes
        // Checking first child in advance in order to get its indexes
        {
            if (newChildren[0]->op == EXTRACT) {
                uint64_t firstBit = newChildren[0]->children->at(1)->cst[0];
                uint64_t currentBit = newChildren[0]->children->at(0)->cst[0] + 1;

                for (int32_t childNum = 1; childNum < (int32_t) newChildren.size(); childNum += 1) {
                    Node & child = *newChildren[childNum];
                    if (child.op == EXTRACT && child.children->at(1)->cst[0] == currentBit) {
                        currentBit = child.children->at(0)->cst[0] + 1;
                    }
                    else {
                        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
                    }
                }
                // Checking if all children are equivalent expressions
                for (int32_t childNum = 1; childNum < (int32_t) newChildren.size(); childNum += 1) {
                    if (!equivalence(*newChildren[0]->children->at(2), *newChildren[childNum]->children->at(2))) {
                        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
                    }
                }

                if (firstBit == 0 && (int32_t) currentBit == newChildren[0]->children->at(2)->width) {
                    return setSimpEqAndReturn(node, *newChildren[0]->children->at(2));
                }
                else {
                    return setSimpEqAndReturn(node, Extract(currentBit - 1, firstBit, *newChildren[0]->children->at(2)));
                }
            }
        }
        #if EXTENDED_MERGE_CONCAT
        {
            bool ok;
            Node * mergedNode = extendedMergeConcatExtract(newChildren, &ok);
            if (ok) {
                return setSimpEqAndReturn(node, *mergedNode);
            }
            else {
                return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
            }
        }
        #endif
        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
    }


    else if (op == SASHR) {
        Node & val = *newChildren[0];
        Node & shval = *newChildren[1];
        if (val.nature == CONST) {
            if (isZero(val.cst, val.width) or isAllOne(val.cst, val.width)) {
                return setSimpEqAndReturn(node, val);
            }
        }
        if (shval.nature == CONST) {
            if (isZero(shval.cst, shval.width)) {
                return setSimpEqAndReturn(node, val);
            }
            else {
                if (val.op == ASHR) {
                    Node & gChild = *val.children->at(0);
                    Node & sh1 = *val.children->at(1);
                    return setSimpEqAndReturn(node, gChild >> (shval.cst[0] + sh1.cst[0]));
                }
                // Note: not calling defaultNode because it would keep the width of the shifted value instead of taking the default (min) one
                return setSimpEqAndReturn(node, val >> shval);
            }
        }
        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
    }


    else if (op == SLSHL) {
        Node & val = *newChildren[0];
        Node & shval = *newChildren[1];
        if (val.nature == CONST) {
            if (isZero(val.cst, val.width)) {
                return setSimpEqAndReturn(node, val);
            }
        }
        if (shval.nature == CONST) {
            if (isZero(shval.cst, shval.width)) {
                return setSimpEqAndReturn(node, val);
            }
            else {
                if (val.op == LSHL) {
                    Node & gChild = *val.children->at(0);
                    Node & sh1 = *val.children->at(1);
                    return setSimpEqAndReturn(node, gChild << (shval.cst[0] + sh1.cst[0]));
                }
                // Note: not calling defaultNode because it would keep the width of the shifted value instead of taking the default (min) one
                return setSimpEqAndReturn(node, val << shval);
            }
        }
        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
    }


    else if (op == SLSHR) {
        Node & val = *newChildren[0];
        Node & shval = *newChildren[1];
        if (val.nature == CONST) {
            if (isZero(val.cst, val.width)) {
                return setSimpEqAndReturn(node, val);
            }
        }
        if (shval.nature == CONST) {
            if (isZero(shval.cst, shval.width)) {
                return setSimpEqAndReturn(node, val);
            }
            else {
                if (val.op == LSHR) {
                    Node & gChild = *val.children->at(0);
                    Node & sh1 = *val.children->at(1);
                    return setSimpEqAndReturn(node, LShR(gChild, (shval.cst[0] + sh1.cst[0])));
                }
                // Note: not calling defaultNode because it would keep the width of the shifted value instead of taking the default (min) one
                return setSimpEqAndReturn(node, LShR(val, shval));
            }
        }
        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
    }


    else if (op == IMUL || op == GMUL) {
        bool m = mergeWithChildrenIfPossible(op, newChildren);
        modified = modified || m;

        // Constant propagation
        int32_t i = 0;
        uint64_t constVal = 1;
        while (i < (int32_t) newChildren.size()) {
            Node & child = *newChildren[i];
            if (child.nature == CONST) {
                assert(child.nlimbs == 1);
                if (op == IMUL) {
                    constVal = imulInt(constVal, child.cst[0], child.width);
                }
                else {
                    constVal = gmulInt(constVal, child.cst[0]);
                }
                newChildren.erase(newChildren.begin() + i);
                modified = true;
                continue;
            }
            i += 1;
        }
        if (constVal == 0) {
            return setSimpEqAndReturn(node, Const(0, width));
        }
        if (constVal != 1) {
            newChildren.push_back(&Const(constVal, width));
        }

        if (op == GMUL) {
            // GMul(GExp(x), GExp(y)) -> GExp(x + y)
            // GMul(GPow(x, n), GPow(x, m) -> GPow(x, n + m)
            // GMul(a, GPow(a, n)) -> GPow(a, n + 1)
            // GMul(a, a) -> GPow(a, 2)

            int32_t nbExpChildren = 0;
            std::map<Node *, int32_t> nbPowChildren;
            for (int32_t i = 0; i < (int32_t) newChildren.size(); i += 1) {
                Node & child = *newChildren[i];
                if (child.op == GEXP) {
                    nbExpChildren += 1;
                }
                else if (child.op == GPOW) {
                    bool exists = false;
                    for (const auto & [exp, val] : nbPowChildren) {
                        if (equivalence(*exp, *child.children->at(0))) {
                            nbPowChildren[exp] += 1;
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {
                        nbPowChildren[child.children->at(0)] = 1;
                    }
                }
                else {
                    bool exists = false;
                    for (const auto & [exp, val] : nbPowChildren) {
                        if (equivalence(*exp, child)) {
                            nbPowChildren[exp] += 1;
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {
                        nbPowChildren[&child] = 1;
                    }
                }
            }


            if (nbExpChildren > 1) {
                modified = true;
                std::vector<Node *> expChildren;
                int32_t i = 0;
                while (i < (int32_t) newChildren.size()) {
                    Node & child = *newChildren[i];
                    if (child.op == GEXP) {
                        expChildren.push_back(child.children->at(0));
                        newChildren.erase(newChildren.begin() + i);
                        continue;
                    }
                    i += 1;
                }
                // QM FIXME: what if usbv is used?
                Node & newAddNode = simplify(Node::OpNode(PLUS, expChildren));
                Node & newExpNode = simplify(GExp(newAddNode));
                newChildren.push_back(&newExpNode);
            }


            for (const auto & [exp, val] : nbPowChildren) {
                if (val > 1) {
                    modified = true;
                    std::vector<Node *> powChildren;
                    int32_t i = 0;
                    while (i < (int32_t) newChildren.size()) {
                        Node & child = *newChildren[i];
                        if (child.op == GPOW && equivalence(*exp, *child.children->at(0))) {
                            powChildren.push_back(child.children->at(1));
                            newChildren.erase(newChildren.begin() + i);
                            continue;
                        }
                        else if (equivalence(*exp, child)) {
                            powChildren.push_back(&Const(1, 8));
                            newChildren.erase(newChildren.begin() + i);
                            continue;
                        }
                        i += 1;
                    }
                    // QM FIXME: what if usbv is used?
                    Node & newAddNode = simplify(Node::OpNode(PLUS, powChildren));
                    Node & newPowNode = simplify(GPow(*exp, newAddNode));
                    newChildren.push_back(&newPowNode);
                }
            }

            // GMul(cst, GLog(x)) -> GLog(GPow(x, cst))
            // Note: this transformation does not respect the reduction in number of operators
            // In case of:
            // cst0 * e0 * log(x) * e1 * cst1 * log(y)
            // The expected result is:
            // Constant propagation of cst0 and cst1 into cst
            // e0 * log(pow(x, cst)) * e1 * log(y)
            // the choice of log(x) w.r.t. log(y) here is arbitrary
            int32_t cstChild = -1;
            int32_t logChild = -1;
            for (int32_t i = 0; i < (int32_t) newChildren.size(); i += 1) {
                if (newChildren[i]->nature == CONST) {
                    cstChild = i;
                }
                if (newChildren[i]->op == GLOG) {
                    logChild = i;
                }
                if (cstChild != -1 && logChild != -1) {
                    break;
                }
            }
            if (cstChild != -1 && logChild != -1) {
                // QM FIXME: what if usbv is used?
                Node & newPowNode = simplify(GPow(*newChildren[logChild]->children->at(0), *newChildren[cstChild]));
                Node & newLogNode = simplify(GLog(newPowNode));
                newChildren.erase(newChildren.begin() + std::max(cstChild, logChild));
                newChildren.erase(newChildren.begin() + std::min(cstChild, logChild));
                newChildren.push_back(&newLogNode);
            }
        }


        if (newChildren.size() == 0) {
            return setSimpEqAndReturn(node, Const(0, width));
        }
        if (newChildren.size() == 1) {
            return setSimpEqAndReturn(node, *newChildren[0]);
        }
        else {
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }


    else if (op == GEXP) {
        Node & child = *newChildren[0];
        if (child.nature == CONST) {
            return gexp(child);
        }
        if (child.op == GLOG) {
            return setSimpEqAndReturn(node, *child.children->at(0));
        }
        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
    }


    else if (op == GLOG) {
        Node & child = *newChildren[0];
        if (child.nature == CONST) {
            return glog(child);
        }
        if (child.op == GEXP) {
            return setSimpEqAndReturn(node, *child.children->at(0));
        }
        else {
            return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
        }
    }


    else if (op == GPOW) {
        Node & c0 = *newChildren[0];
        Node & c1 = *newChildren[1];
        if (c1.nature == CONST) {
            if (c0.nature == CONST) {
                return gpow(c0, c1);
            }
            else if (c1.cst[0] == 1) {
                return setSimpEqAndReturn(node, c0);
            }
        }
        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
    }
     

    // Default case
    else {
        return setSimpEqAndReturn(node, defaultNode(node, op, newChildren, modified));
    }
}


bool equivalence(Node & node0, Node & node1) {
    Node & node0simp = simplify(node0);
    Node & node1simp = simplify(node1);
    for (int32_t i = 0; i < 4; i += 1) {
        if (node0simp.h[i] != node1simp.h[i]) {
            return false;
        }
    }
    return true;
}





