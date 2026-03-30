/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#include <set>
#include <vector>
#include <algorithm>
#include <cassert>

#include "tps.hpp"
#include "node.hpp"
#include "simplify.hpp"
#include "concrev.hpp"
#include "utils_private.hpp"



std::string secProp2str(SecurityProperty secProp) {
    switch (secProp) {
        case PROP_NONE:
            return "None";
        case TPS:
            return "TPS";
        case NI:
            return "NI";
        case SNI:
            return "SNI";
        case RNI:
            return "RNI";
        case PINI:
            return "PINI";
        case OPINI:
            return "OPINI";
        case OPINI_1ST_CALL:
            return "OPINI_1ST_CALL";
        case OPINI_2ND_CALL:
            return "OPINI_2ND_CALL";
        default:
            assert(false);
    }
}




static Node & getReplacedGraphRec(Node & node, Node & selMask, Node & childToReplace, std::map<Node *, Node *> & m) {
    if (node.nature != OP) {
        return node;
    }
    std::vector<Node *> children;
    for (const auto & child : *node.children) {
        if (m.contains(child)) {
            children.push_back(m[child]);
            continue;
        }
        if (child == &selMask) {
            children.push_back(&childToReplace);
            continue;
        }
        if (child == &childToReplace) {
            children.push_back(&selMask);
            continue;
        }
        if (child->maskingMaskOcc->contains(&selMask) || child->otherMaskOcc->contains(&selMask)) {
            Node & newChild = getReplacedGraphRec(*child, selMask, childToReplace, m);
            children.push_back(&newChild);
        }
        else {
            children.push_back(child);
        }
    }
    Node & n = Node::OpNode(node.op, children);
    m[&node] = &n;
    return n;
}


static Node & getReplacedGraph(Node & node, Node & selMask, Node & childToReplace) {
    std::map<Node *, Node *> m;
    return getReplacedGraphRec(node, selMask, childToReplace, m);
}


static Node & removeConstOperandsFromPlusRec(Node & node, std::map<Node *, Node *> & m) {
    if (node.nature != OP or !node.hasPlus) {
        return node;
    }
    std::vector<Node *> children;
    for (const auto & child : *node.children) {
        if (m.contains(child)) {
            children.push_back(m[child]);
            continue;
        }
        if (node.op == PLUS and child->nature == CONST) {
            continue;
        }
        Node & newChild = removeConstOperandsFromPlusRec(*child, m);
        children.push_back(&newChild);
    }
    if (children == *node.children) {
        m[&node] = &node;
        return node;
    }
    if (children.size() == 0) {
        assert(false);
        Node & n = Const(0, node.width);
        m[&node] = &n;
        return n;
    }
    if (node.children->size() != children.size() && children.size() == 1) {
        assert(node.op == PLUS);
        Node & n = *children[0];
        m[&node] = &n;
        return n;
    }
    Node & n = Node::OpNode(node.op, children);
    m[&node] = &n;
    return n;
}


static Node & removeConstOperandsFromPlus(Node & node) {
    if (not node.hasPlus) {
        return node;
    }
    else {
        //std::cout << "RCOFP: " << node << std::endl;
        std::map<Node *, Node *> m;

        Node & res = removeConstOperandsFromPlusRec(node, m);
        //std::cout << "Res  : " << res << std::endl;
        if (&res != &node) {
            //std::cout << "Calling simplify here" << std::endl;
            return simplify(res);
        }
        return node;
    }
}



static bool checkProperty(Node & nodeIn, SecurityProperty secProp, PropParams & params, bool noFalsePositive, bool verbose) {
    if (verbose) {
        std::cout << "# Call func checkProperty on exp " << nodeIn << std::endl;
    }
    
    assert(not noFalsePositive or (secProp == TPS or secProp == NI or secProp == PINI)) ;

    int maxShareOcc = 0;
    int diff;
    std::set<int> * outputIndexes = NULL;
    std::set<int> * additionalInputIndexes = NULL;

    if (secProp == NI) {
        maxShareOcc = params.maxShareOcc;
    }
    else if (secProp == RNI) {
        diff = params.diff; // Difference between the order of verification and the n-uplet size
    }
    else if (secProp == PINI) {
        maxShareOcc = params.maxShareOcc;
        outputIndexes = params.outputIndexes;
    }
    else if (secProp == OPINI_1ST_CALL || secProp == OPINI_2ND_CALL) {
        maxShareOcc = params.maxShareOcc;
        outputIndexes = params.outputIndexes;
        additionalInputIndexes = params.additionalInputIndexes;
    }


    Node * node = &nodeIn;
    // Simplify made in wrapper
    #if VERIF_REMOVE_CST_IN_ADD
    node = &removeConstOperandsFromPlus(*node);
    #endif

    std::set<Node *> masksTaken;
    int cpt = 0;
    while (true) {
        if (verbose) {
            std::cout << "# Starting iteration " << cpt << std::endl;
            std::cout << "# e = " << *node << std::endl;
        }
        cpt += 1;

        if (secProp == TPS) {
            if (node->secretVarOcc->size() == 0) {
                if (verbose) {
                    std::cout << "# No more secret" << std::endl;
                }
                return true;
            }
        }
        else if (secProp == NI) {
            bool isNI = true;
            for (const auto & [s, val] : *node->shareOcc) {
                // val is node->shareOcc->at(s)
                //std::cout << "origSecret: " << *s << std::endl;
                //std::cout << "val->size : " << val->size() << " - maxShareOcc : " << maxShareOcc << std::endl;
                if ((int32_t) val->size() > maxShareOcc) {
                    isNI = false;
                    break;
                }
            }
            if (isNI) {
                return true;
            }
        }
        else if (secProp == RNI) {
            bool isRNI = true;
            for (const auto & [s, shares] : *node->shareOcc) {
                // share is node->shareOcc->at(s)
                int nbShares = -1;
                for (const auto & [anyOcc, val] : *shares) {
                    nbShares = anyOcc->nbShares;
                    break;
                }
                if ((int32_t) shares->size() >= nbShares - diff) {
                    isRNI = false;
                    break;
                }
            }
            if (isRNI) {
                return true;
            }
        }
        else if (secProp == PINI) {
            //print('# outputIndexes: %s' % ' '.join(map(lambda x: '%d' % x, outputIndexes)))
            std::set<int32_t> internalSharesIndexes;
            for (const auto & [s, shares] : *node->shareOcc) {
                for (const auto & [sh, val] : *shares) {
                    int32_t num = sh->shareNum;
                    if (not outputIndexes->contains(num)) {
                        internalSharesIndexes.insert(num);
                    }
                }
            }
            if ((int32_t) internalSharesIndexes.size() <= maxShareOcc) {
                return true;
            }
        }
        else if (secProp == OPINI_1ST_CALL) {
            //print('# outputIndexes: %s' % ' '.join(map(lambda x: '%d' % x, outputIndexes)))
            std::set<int32_t> internalSharesIndexes;
            for (const auto & [s, shares] : *node->shareOcc) {
                for (const auto & [sh, val] : *shares) {
                    int32_t num = sh->shareNum;
                    if (not outputIndexes->contains(num)) {
                        internalSharesIndexes.insert(num);
                    }
                }
            }
            if ((int32_t) internalSharesIndexes.size() <= maxShareOcc) {
                // FIXME: possible to return true here whereas more replacements could be made, in a way that the 2nd check will fail but would have succeeded if all replacements had been made
                // We now need to check that we can also simulate the output share indexes with the same indexes as those used for input shares
                for (int32_t num : internalSharesIndexes) {
                    additionalInputIndexes->insert(num);
                }
                return true;
            }
        }
        else if (secProp == OPINI_2ND_CALL) {
            bool isOPini = true;
            std::set<int32_t> internalSharesIndexes;
            for (const auto & [s, shares] : *node->shareOcc) {
                for (const auto & [sh, val] : *shares) {
                    int32_t num = sh->shareNum;
                    if (not outputIndexes->contains(num)) {
                        isOPini = false;
                    }
                }
            }
            if (isOPini) {
                return true;
            }
        }


        if (node->currentlyMasking->size() != 0) {
            if (verbose) {
                std::cout << "# At least one node is currently masking" << std::endl;
            }
            return true;
        }
 
        // Choice of CTR:
        // - Choose mask m which minimizes the number of nodes with occurrence of m (CTR Bases + other Occ)
        // - For this m, choose CTR Base with the highest count
        // - For this CTR Base, choose the CTR with the max height for the same count
        // FIXME: change map copies to an alias, check that this is ok
        std::map<Node *, std::map<Node *, std::map<Node *, std::pair<uint64_t, int32_t>> * > * > & maskingMaskOcc = *node->maskingMaskOcc;
        #if SEL_MSK_W_NON_MSKNG_OCC
        std::map<Node *, std::set<Node *> * > & otherMaskOcc = *node->otherMaskOcc;
        #else
        std::set<Node *> & otherMaskOcc = *node->otherMaskOcc;
        #endif
        int32_t minOtherOcc = 1000000;
        int32_t minMaskingOcc = 1000000;
        bool minRootMask = false;
        Node * selMask = NULL;
        for (const auto & [m, v] : maskingMaskOcc) {
            int32_t nbOtherOcc = 0;

            int32_t nbMaskingOcc = maskingMaskOcc[m]->size();

            #if SEL_MSK_W_NON_MSKNG_OCC
            if (otherMaskOcc.contains(m)) {
                nbOtherOcc = otherMaskOcc[m]->size();
            }
            #else
            if (otherMaskOcc.contains(m)) {
                continue;
            }
            #endif

            // FIXME: change the condition nbMaskingOcc == 1 with the fact that the number of maskingOcc has strictly decreased since the last time the mask was taken?
            if (masksTaken.contains(m) && !(nbMaskingOcc == 1 && nbOtherOcc == 0)) {
                continue;
            }
            
            bool rootMask = (node->op == CONCAT && std::find(node->children->begin(), node->children->end(), m) != node->children->end());

            // Heuristic:
            // 1. First, minimize the number of otherOcc
            // 2. For the masks with a min number of otherOcc, minimize the number of maskingOcc
            // (Old heuristic: minimize total number of occurrences)
            // if SEL_MSK_W_MSKNG_OCC is false, nbOtherOcc is always 0 if the mask is taken, so we only minimize the number of maskingOcc
            if ((!rootMask && minRootMask) || (rootMask == minRootMask && (nbOtherOcc < minOtherOcc || (nbOtherOcc == minOtherOcc && nbMaskingOcc < minMaskingOcc)))) {
                selMask = m;
                minRootMask = rootMask;
                minOtherOcc = nbOtherOcc;
                minMaskingOcc = nbMaskingOcc;
            }
        }

        if (selMask == NULL) {
            if (verbose) {
                std::cout << "# No mask can be taken" << std::endl;
            }
            if (noFalsePositive) {
                if (verbose) {
                    std::cout << "# NoFalsePositive enabled, calling enumeration" << std::endl;
                }
                if (secProp == TPS) {
                    return isTPSWithExev(*node);
                }
                else if (secProp == NI) {
                    return isNIWithExev(*node, maxShareOcc);
                }
                else if (secProp == PINI) {
                    return isPINIWithExev(*node, maxShareOcc, *outputIndexes);
                }
            }
            return false;
        }

        if (verbose) {
            std::cout << "# Choosing mask " << *selMask << " (number of parent nodes: " << (minMaskingOcc + minOtherOcc) << ")" << std::endl;
        }

        uint64_t minCount = std::numeric_limits<uint64_t>::max();
        Node * selCtrBase = NULL;
        std::map<Node *, std::map<Node *, std::pair<uint64_t, int32_t>> * > & occs = *maskingMaskOcc[selMask];
        for (const auto & [ctrBase, val] : occs) { // val is occs[ctrBase]
            if (val->at(ctrBase).first < minCount) {
                minCount = val->at(ctrBase).first;
                selCtrBase = ctrBase;
            }
        }
        if (selCtrBase == NULL) {
            for (const auto & [ctrBase, val] : occs) { // val is occs[ctrBase]
                selCtrBase = ctrBase;
                break;
            }
        }


        if (verbose) {
            std::cout << "# Choosing following ctr base with " << minCount << " occurrences: " << *selCtrBase << std::endl;
        }

        int32_t maxHeight = -1;
        Node * selCtr = NULL;
        if (minCount == std::numeric_limits<uint64_t>::max()) {
            selCtr = selCtrBase;
        }
        else {
            for (const auto & [ctr, val] : *occs[selCtrBase]) { // val is occs[selCtrBase][ctr]
                int32_t height = val.second;
                if (val.first == minCount && height > maxHeight) {
                    maxHeight = height;
                    selCtr = ctr;
                }
            }
        }
        assert(selCtr != NULL);
        
        if (verbose) {
            std::cout << "# Choosing following ctr with a height of " << maxHeight << ": " << *selCtr << std::endl;
        }

        // FIXME: currently the results is not deterministic, and a some verifications can either
        //        succeed or fail, even if it is a very rare case
        //        There are three non deterministic choices, and in order to make them deterministic,
        //        we can for example take the min or max of the node number
        //        I still expect this to slow down the execution time a little...

        masksTaken.insert(selMask);
        node = &getReplacedGraph(*node, *selMask, *selCtr);

        if (verbose) {
            std::cout << "# Replacing " << *selCtr << " with " << *selMask << std::endl;
            std::cout << "# and other occurrences of " << *selMask << " with " << *selCtr << std::endl;
        }

        // Simplify
        node = &simplify(*node);
        #if VERIF_REMOVE_CST_IN_ADD
        node = &removeConstOperandsFromPlus(*node);
        #endif
    }
}


static bool checkPropertyWrapper(Node & nodeIn, SecurityProperty secProp, PropParams & params, bool bitDecompose, bool noFalsePositive, bool verbose) {
    Node * n = &simplify(nodeIn);
    if (bitDecompose) {
        n = &getBitDecomposition(*n);
    }
    Node::setModeTemporaryNodes();
    bool res = checkProperty(*n, secProp, params, noFalsePositive, verbose);
    Node::setModePermanentNodes();
    return res;
}


static bool checkPropertyWrapper(std::vector<Node *> & nodesIn, SecurityProperty secProp, PropParams & params, bool bitDecompose, bool noFalsePositive, bool verbose) {
    std::vector<Node *> nodes;
    for (const auto & n : nodesIn) {
        nodes.push_back(&simplify(*n));
    }
    if (bitDecompose) {
        std::vector<Node *> nodesBE;
        for (const auto & n : nodes) {
            nodesBE.push_back(&getBitDecomposition(*n));
        }
        nodes = nodesBE;
    }

    Node::setModeTemporaryNodes();
    Node & c = simplify(Concat(nodes));
    bool res = checkProperty(c, secProp, params, noFalsePositive, verbose);
    Node::setModePermanentNodes();
    return res;
}


bool tps(Node & nodeIn, bool bitDecompose, bool verbose) {
    tpsValidity(nodeIn);
    PropParams dummy;
    return checkPropertyWrapper(nodeIn, TPS, dummy, bitDecompose, false, verbose);
}


bool tps(std::vector<Node *> & nodes, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        tpsValidity(*n);
    }
    PropParams dummy;
    return checkPropertyWrapper(nodes, TPS, dummy, bitDecompose, false, verbose);
}


bool tpsNoFalsePositive(Node & nodeIn, bool bitDecompose, bool verbose) {
    tpsValidity(nodeIn);
    PropParams dummy;
    return checkPropertyWrapper(nodeIn, TPS, dummy, bitDecompose, true, verbose);
}


bool tpsNoFalsePositive(std::vector<Node *> & nodes, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        tpsValidity(*n);
    }
    PropParams dummy;
    return checkPropertyWrapper(nodes, TPS, dummy, bitDecompose, true, verbose);
}



bool ni(Node & nodeIn, int maxShareOcc, bool bitDecompose, bool verbose) {
    niValidity(nodeIn);
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    return checkPropertyWrapper(nodeIn, NI, pp, bitDecompose, false, verbose);
}


bool ni(std::vector<Node *> & nodes, int maxShareOcc, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        niValidity(*n);
    }
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    return checkPropertyWrapper(nodes, NI, pp, bitDecompose, false, verbose);
}


bool niNoFalsePositive(Node & nodeIn, int maxShareOcc, bool bitDecompose, bool verbose) {
    niValidity(nodeIn);
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    return checkPropertyWrapper(nodeIn, NI, pp, bitDecompose, true, verbose);
}


bool niNoFalsePositive(std::vector<Node *> & nodes, int maxShareOcc, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        niValidity(*n);
    }
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    return checkPropertyWrapper(nodes, NI, pp, bitDecompose, true, verbose);
}



bool rni(Node & nodeIn, int diff, bool bitDecompose, bool verbose) {
    rniValidity(nodeIn);
    PropParams pp;
    pp.diff = diff;
    return checkPropertyWrapper(nodeIn, RNI, pp, bitDecompose, false, verbose);
}


bool rni(std::vector<Node *> & nodes, int diff, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        rniValidity(*n);
    }
    PropParams pp;
    pp.diff = diff;
    return checkPropertyWrapper(nodes, RNI, pp, bitDecompose, false, verbose);
}


bool pini(Node & nodeIn, int maxShareOcc, std::set<int> & outputIndexes, bool bitDecompose, bool verbose) {
    piniValidity(nodeIn);
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    pp.outputIndexes = &outputIndexes;
    return checkPropertyWrapper(nodeIn, PINI, pp, bitDecompose, false, verbose);
}


bool pini(std::vector<Node *> & nodes, int maxShareOcc, std::set<int> & outputIndexes, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        piniValidity(*n);
    }
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    pp.outputIndexes = &outputIndexes;
    return checkPropertyWrapper(nodes, PINI, pp, bitDecompose, false, verbose);
}


bool piniNoFalsePositive(Node & nodeIn, int maxShareOcc, std::set<int> & outputIndexes, bool bitDecompose, bool verbose) {
    piniValidity(nodeIn);
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    pp.outputIndexes = &outputIndexes;
    return checkPropertyWrapper(nodeIn, PINI, pp, bitDecompose, true, verbose);
}


bool piniNoFalsePositive(std::vector<Node *> & nodes, int maxShareOcc, std::set<int> & outputIndexes, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        piniValidity(*n);
    }
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    pp.outputIndexes = &outputIndexes;
    return checkPropertyWrapper(nodes, PINI, pp, bitDecompose, true, verbose);
}


bool opini(Node & nodeIn, int maxShareOcc, std::set<int> & outputIndexes, std::vector<std::vector<Node *> *> & allOutputsLeakages, bool bitDecompose, bool verbose) {
    piniValidity(nodeIn);
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    pp.outputIndexes = &outputIndexes;
    std::set<int> additionalInputIndexes;
    pp.additionalInputIndexes = &additionalInputIndexes;
    bool res = checkPropertyWrapper(nodeIn, OPINI_1ST_CALL, pp, bitDecompose, false, verbose);
    if (!res) {
        return false;
    }
    pp.maxShareOcc = 0; // No more additional shares authorized since all authorized indexes correspond to an output index
    std::vector<Node *> probesLeakagesWithNewOutputs;
    probesLeakagesWithNewOutputs.push_back(&nodeIn);
    for (int idx : additionalInputIndexes) {
        outputIndexes.insert(idx);
        for (const auto & output : allOutputsLeakages) {
            probesLeakagesWithNewOutputs.push_back(output->at(idx));
        }
    }
    pp.additionalInputIndexes = NULL;
    return checkPropertyWrapper(probesLeakagesWithNewOutputs, OPINI_2ND_CALL, pp, bitDecompose, false, verbose);
}


bool opini(std::vector<Node *> & nodes, int maxShareOcc, std::set<int> & outputIndexes, std::vector<std::vector<Node *> *> & allOutputsLeakages, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        piniValidity(*n);
    }
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    pp.outputIndexes = &outputIndexes;
    std::set<int> additionalInputIndexes;
    pp.additionalInputIndexes = &additionalInputIndexes;
    bool res = checkPropertyWrapper(nodes, OPINI_1ST_CALL, pp, bitDecompose, false, verbose);
    if (!res) {
        return false;
    }
    pp.maxShareOcc = 0;
    for (int idx : additionalInputIndexes) {
        outputIndexes.insert(idx);
        for (const auto & output : allOutputsLeakages) {
            nodes.push_back(output->at(idx));
        }
    }
    pp.additionalInputIndexes = NULL;
    return checkPropertyWrapper(nodes, OPINI_2ND_CALL, pp, bitDecompose, false, verbose);
}


bool opiniNoFalsePositive(Node & nodeIn, int maxShareOcc, std::set<int> & outputIndexes, std::vector<std::vector<Node *> *> & allOutputsLeakages, bool bitDecompose, bool verbose) {
    piniValidity(nodeIn);
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    pp.outputIndexes = &outputIndexes;
    std::set<int> additionalInputIndexes;
    pp.additionalInputIndexes = &additionalInputIndexes;
    bool res = checkPropertyWrapper(nodeIn, OPINI_1ST_CALL, pp, bitDecompose, true, verbose);
    if (!res) {
        return false;
    }
    pp.maxShareOcc = 0;
    std::vector<Node *> probesLeakagesWithNewOutputs;
    probesLeakagesWithNewOutputs.push_back(&nodeIn);
    for (int idx : additionalInputIndexes) {
        outputIndexes.insert(idx);
        for (const auto & output : allOutputsLeakages) {
            probesLeakagesWithNewOutputs.push_back(output->at(idx));
        }
    }
    pp.additionalInputIndexes = NULL;
    return checkPropertyWrapper(probesLeakagesWithNewOutputs, OPINI_2ND_CALL, pp, bitDecompose, true, verbose);
}


bool opiniNoFalsePositive(std::vector<Node *> & nodes, int maxShareOcc, std::set<int> & outputIndexes, std::vector<std::vector<Node *> *> & allOutputsLeakages, bool bitDecompose, bool verbose) {
    for (const auto & n : nodes) {
        piniValidity(*n);
    }
    PropParams pp;
    pp.maxShareOcc = maxShareOcc;
    pp.outputIndexes = &outputIndexes;
    std::set<int> additionalInputIndexes;
    pp.additionalInputIndexes = &additionalInputIndexes;
    bool res = checkPropertyWrapper(nodes, OPINI_1ST_CALL, pp, bitDecompose, true, verbose);
    if (!res) {
        return false;
    }
    pp.maxShareOcc = 0;
    for (int idx : additionalInputIndexes) {
        outputIndexes.insert(idx);
        for (const auto & output : allOutputsLeakages) {
            nodes.push_back(output->at(idx));
        }
    }
    pp.additionalInputIndexes = NULL;
    return checkPropertyWrapper(nodes, OPINI_2ND_CALL, pp, bitDecompose, true, verbose);
}


