/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#include <map>
#include <deque>
#include <cassert>


#include "simp_rules.hpp"
#include "config.hpp"
#include "node.hpp"
#include "simplify.hpp"
#include "concrev.hpp"


std::map<std::tuple<uint64_t, uint64_t, uint64_t, uint64_t>, std::set<std::tuple<Node *, Node *>>> allRules;


static bool checkRule(Node & s) {
    if (s.op == IMUL || s.op == GMUL) {
        return false;
    }
    if (s.children == NULL) {
        return true;
    }
    for (const auto & c : *s.children) {
        bool res = checkRule(*c);
        if (!res) {
            return false;
        }
    }
    return true;
}


 


static bool enumMappingsRec(std::map<Node *, Node *> & m, std::deque<Node *> & exps, std::deque<Node *> & origs, std::deque<int32_t> & childNums, std::deque<std::deque<bool>> & taken, std::deque<int32_t> & nbRemainingChildren) {


    Node & exp = *exps[0];
    Node & orig = *origs[0];

    std::deque<bool> & childrenTaken = taken[0];

    if (nbRemainingChildren[0] == 0) {
        int32_t idx = 1;
        while (idx < (int32_t) origs.size() and (childNums[idx] == (int32_t) origs[idx]->children->size() or (origs[idx]->children->at(childNums[idx]) != &orig))) {
            idx += 1;
        }

        if (idx == (int32_t) origs.size()) {
            return true;
        }

        exps.push_front(exps[idx]);
        origs.push_front(origs[idx]);

        if (nbRemainingChildren[idx] > 0) {
            childNums.push_front(childNums[idx] + 1);
            nbRemainingChildren.push_front(nbRemainingChildren[idx] - 1);
        }
        else {
            assert(nbRemainingChildren[idx] == 0);
            childNums.push_front(childNums[idx]);
            nbRemainingChildren.push_front(0);
        }

        taken.push_front(taken[idx]);

        bool res = enumMappingsRec(m, exps, origs, childNums, taken, nbRemainingChildren);
        if (res) {
            return true;
        }

        exps.pop_front();
        origs.pop_front();
        childNums.pop_front();
        taken.pop_front();
        nbRemainingChildren.pop_front();

        return false;
    }

    int32_t childNum = childNums[0];

    Node & child = *orig.children->at(childNum);

    for (int32_t i = 0; i < nbRemainingChildren[0]; i += 1) {
        // Taking the i-th non taken symbol
        int32_t j = 0;
        int32_t k = 0;
        while (childrenTaken[k]) {
            k += 1;
        }
        while (j != i) {
            j += 1;
            k += 1;
            while (childrenTaken[k]) {
                k += 1;
            }
        }

        if (child.h == exp.children->at(k)->h) {
            if (child.nature == SYMB) {
                if (not m.contains(&child)) {
                    m.emplace(&child, exp.children->at(k));

                    childrenTaken[k] = true;

                    childNums[0] += 1;
                    nbRemainingChildren[0] -= 1;
                    bool res = enumMappingsRec(m, exps, origs, childNums, taken, nbRemainingChildren);
                    if (res) {
                        return true;
                    }

                    nbRemainingChildren[0] += 1;
                    childNums[0] -= 1;

                    childrenTaken[k] = false;
                    m.erase(&child);
                }
                else if (m[&child] == exp.children->at(k)) {
                    childrenTaken[k] = true;
                    childNums[0] += 1;
                    nbRemainingChildren[0] -= 1;
                    bool res = enumMappingsRec(m, exps, origs, childNums, taken, nbRemainingChildren);
                    if (res) {
                        return true;
                    }
                    nbRemainingChildren[0] += 1;
                    childNums[0] -= 1;

                    childrenTaken[k] = false;
                }
            }
            else if (child.nature == OP) {
                childrenTaken[k] = true;

                origs.push_front(&child);
                exps.push_front(exp.children->at(k));
                childNums.push_front(0);
                std::deque<bool> initDeque(child.children->size(), false);
                taken.push_front(initDeque);
                nbRemainingChildren.push_front(child.children->size());

                bool res = enumMappingsRec(m, exps, origs, childNums, taken, nbRemainingChildren);
                if (res) {
                    return true;
                }

                origs.pop_front();
                exps.pop_front();
                childNums.pop_front();
                taken.pop_front();
                nbRemainingChildren.pop_front();

                childrenTaken[k] = false;
            }
        }
    }

    return false;
}




static bool getMapping(std::map<Node *, Node *> & m, Node * exp, Node * orig) {
    std::deque<Node *> exps;
    exps.push_front(exp);

    std::deque<Node *> origs;
    origs.push_front(orig);

    std::deque<int32_t> childNums;
    childNums.push_front(0);

    int32_t nbChildren = exp->children->size();
    std::deque<bool> takenInit(nbChildren, false);
    std::deque<std::deque<bool>> taken;
    taken.push_front(takenInit);

    std::deque<int32_t> nbRemainingChildren;
    nbRemainingChildren.push_front(nbChildren);

    bool res = enumMappingsRec(m, exps, origs, childNums, taken, nbRemainingChildren);
    return res;
}



static Node * copyExpRec(std::map<Node *, Node *> & m, Node * trg) {
    if (trg->nature == SYMB) {
        return m[trg];
    }
    if (trg->nature == CONST) {
        return trg;
    }
    if (trg->nature == STR) {
        return trg;
    }

    // OpNode
    std::vector<Node *> children;
    for (const auto & c : *trg->children) {
        Node * newChild = copyExpRec(m, c);
        children.push_back(newChild);
    }
    Node & n = Node::OpNode(trg->op, children);
    return &n;
}


static Node * copyExp(Node * e, Node * orig, Node * target) {
    std::map<Node *, Node *> m;
    bool res = getMapping(m, e, orig);
    if (!res) {
        return NULL;
    }

    Node * retNode = copyExpRec(m, target);
    return retNode;
}


static Node * getExpStructureRec(Node * n, std::map<Node *, Node *> & m) {
    if (n->nature == SYMB) {
        if (m.contains(n)) {
            return m[n];
        }
        else {
            Node & newSymbNode = SymbInternal(std::string("%v"), 'P', n->width);
            m[n] = &newSymbNode;
            return &newSymbNode;
        }
    }
    if (n->nature == CONST || n->nature == STR) {
        return n;
    }

    // OpNode
    std::vector<Node *> children;
    for (const auto & c : *n->children) {
        Node * newChild = getExpStructureRec(c, m);
        children.push_back(newChild);
    }
    Node & r = Node::OpNode(n->op, children);
    return &r;
}


Node & getExpStructure(Node & n) {
    std::map<Node *, Node *> m;
    return *getExpStructureRec(&n, m);
}



void addSpecificSimpRule(Node & s, Node & t) {
    Node & ss = simplify(s);
    s.simpEq = &t;
    ss.simpEq = &t;
    if (getMaxSymbVarWidth(s) == 1) {
        s.simpEqUsbv = &t;
        ss.simpEqUsbv = &t;
    }
}


void addGenericSimpRule(Node & s, Node & t) {
    bool ok = checkRule(s);
    if (!ok) {
        std::cerr << "*** Error: Operations \'*\' and Gmul are not supported for simplification rules" << std::endl;
        exit(EXIT_FAILURE);
    }

    Node & ss = simplify(s); // simplifiedSource
    Node & sss = getExpStructure(ss); // SimplifiedSourceStructure

#if 1
    std::tuple<uint64_t, uint64_t, uint64_t, uint64_t> key {sss.h[0], sss.h[1], sss.h[2], sss.h[3]};
    std::tuple<Node*, Node*> val {&ss, &t};
    if (allRules.contains(key)) {
        allRules[key].insert(val);
    }
    else {
        std::set<std::tuple<Node *, Node *>> newSet;
        newSet.insert(val);
        allRules.emplace(key, newSet);
    }
    std::cout << "# Adding key : " << std::get<0>(key) << " -> (" << s << " -> " << t << ")" << std::endl;
#else
    allRules.emplace(std::piecewise_construct, std::forward_as_tuple(ss.h[0], ss.h[1], ss.h[2], ss.h[3]), std::forward_as_tuple(&s, &t));
#endif
    addSpecificSimpRule(s, t);
}


Node & getEquiv(Node & e) {
    bool dummy;
    return getEquiv(e, &dummy);
}


Node & getEquiv(Node & e, bool * found) {
    if (allRules.size() == 0) {
        *found = false;
        return e;
    }

    Node & es = getExpStructure(e); // eStructure

    std::tuple<uint64_t, uint64_t, uint64_t, uint64_t> key {es.h[0], es.h[1], es.h[2], es.h[3]};
    std::cout << "# e   : " << e << std::endl;
    std::cout << "# es  : " << es << std::endl;
    std::cout << "# Key : " << std::hex << std::get<0>(key) << std::dec << std::endl;
    if (allRules.contains(key)) {
        std::cout << "# hash found" << std::endl;
        std::set<std::tuple<Node *, Node *>> rules = allRules[key];
        for (const auto & rule : rules) {
            Node * res = copyExp(&e, std::get<0>(rule), std::get<1>(rule));
            if (res != NULL) {
                *found = true;
                return *res;
            }
        }
    }
    else {
        std::cout << "# hash not found" << std::endl;
    }
    std::cout << "# hash + mapping not found" << std::endl;
    *found = false;
    return e;
}







    
   


