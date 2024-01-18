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
#include <set>
#include <cassert>
#include <functional>
#include <memory>


#include "node.hpp"
#include "config.hpp"
#include "simplify.hpp"
#include "check_leakage.hpp"
#include "hw.hpp"
#include "tps.hpp"


HWElement::HWElement() {
    num = nodeNum;
    nodeNum += 1;
    allHWElements.insert(this);
}


HWElement::~HWElement() {
    //std::cout << "# Deleting HWElement " << num << std::endl;
    //std::cout << "#          symbExp: " << *symbExp << std::endl;
}


static void dumpCircuitGates(const char * filename, std::set<HWElement *> & hwElems) {
    std::string content = "digraph g {\n";
    content += "    rankdir=\"LR\";";
    for (const auto & hwe : hwElems) {
        std::string s;
        if (hwe->nature == GATE) {
            if (hwe->op == BOR) {
                s = std::string("OR Gate (") + std::to_string(hwe->num) + ")\n/" + std::to_string(hwe->symbExp->width) + "/";
            }
            else if (hwe->op == BAND) {
                s = std::string("AND Gate (") + std::to_string(hwe->num) + ")\n/" + std::to_string(hwe->symbExp->width) + "/";
            }
            else if (hwe->op == BXOR) {
                s = std::string("XOR Gate (") + std::to_string(hwe->num) + ")\n/" + std::to_string(hwe->symbExp->width) + "/";
            }
            else if (hwe->op == INPUT) {
                Node & n = *hwe->symbExp;
                if (n.nature == SYMB) {
                    s = std::string("Symbol: ") + *n.symb + " [" + n.symbType + "]" + " (" + std::to_string(hwe->num) + ")\n/" + std::to_string(n.width) + "/";
                }
                else if (n.nature == CONST) {
                    s = std::string("Const: ") + std::to_string(n.cst) + "  (" + std::to_string(hwe->num) + ")\n/" + std::to_string(n.width) + "/";
                }
                else {
                    s = std::string("Exp: ") + n.toString() + " (" + std::to_string(hwe->num) + ")\n/" + std::to_string(n.width) + "/";
                }
            }
            else {
                assert(false);
            }
        }
        else if (hwe->nature == REGISTER) {
            s = std::string("Register (") + std::to_string(hwe->num) + ")\n/" + std::to_string(hwe->symbExp->width) + "/";
        }
        else {
            assert(false);
        }
            
        std::string shape = hwe->nature == REGISTER ? "rectangle" : "oval";
        content += "   N" + std::to_string(hwe->num) + " [shape=" + shape + ", label=\"" + s + "\"];\n";
        for (const auto & a : hwe->inputs) {
            content += "   edge[tailclip=true];\n";
            content += "   N" + std::to_string(a->num) + " -> N" + std::to_string(hwe->num) + "\n";
        }
    }

    content += "}";
    std::ofstream out(filename);
    out << content;
    out.close();
}




HWElement & HWElement::InputGate(Node & input) {
    HWElement * e = new HWElement();
    e->nature = GATE;
    e->op = INPUT;
    e->symbExp = &input;
    e->leakageOut.insert(&input);

    return *e;
}


HWElement & HWElement::Gate(NodeOp op, std::vector<HWElement *> & inputs) {
    HWElement * e = new HWElement();
    e->nature = GATE;
    e->op = op;
    for (const auto & inp : inputs) {
        e->inputs.push_back(inp);
    }
    //e->symbExp = &simplify(Node::makeBitwiseNode(op, input0.symbExp, input1.symbExp));
    std::vector<Node *> symbExps;
    for (const auto & inp : inputs) {
        symbExps.push_back(inp->symbExp);
    }
    e->symbExp = &simplify(Node::OpNode(op, symbExps));
    for (const auto & inp : e->inputs) {
        for (const auto & lo : inp->leakageOut) {
            e->leakageOut.insert(lo);
        }
    }
    return *e;
}


HWElement & HWElement::Register(HWElement & input) {
    HWElement * e = new HWElement();
    e->nature = REGISTER;
    e->op = NOP;
    e->inputs.push_back(&input);
    e->symbExp = input.symbExp;
    e->leakageOut.insert(input.symbExp);
    return *e;
}


std::string HWElement::toString() const {
    if (nature == GATE) {
        std::string s =  std::string("Gate ") + Node::op2strOp(op) + " (" + std::to_string(num) + ")\n";
        s += std::string("   Symbolic Exp: ") + symbExp->toString() + "\n";
        std::string loStr;
        for (bool first{true}; const auto & lo : leakageOut) {
            loStr += (first ? first = false, "" : ", ") + lo->toString();
        }
        s += std::string("   Leakage out: ") + loStr + "\n";
        std::string inStr;
        for (bool first{true}; const auto & in : inputs) {
            inStr += (first ? first = false, "" : ", ") + std::string("Gate ") + std::to_string(in->num);
        }
        s += std::string("   Inputs: ") + inStr + "\n";
        return s;
    }
    else if (nature == REGISTER) {
        std::string s = std::string("Register (") + std::to_string(num) + ")\n";
        s += std::string("   Symbolic Exp: ") + symbExp->toString() + "\n";
        std::string loStr;
        for (bool first{true}; const auto & lo : leakageOut) {
            loStr += (first ? first = false, "" : ", ") + lo->toString();
        }
        s += std::string("   Leakage out:  ") + loStr + "\n";
        s += std::string("   Input:        Gate/Reg ") + std::to_string(inputs[0]->num) + "\n";
        return s;
    }
    else {
        assert(false);
    }
}


std::ostream& operator<<(std::ostream &strm, const HWElement & e) {
    return strm << e.toString();
}



HWElement & andGate(std::vector<HWElement *> & children) {
    return HWElement::Gate(BAND, children);
}


HWElement & orGate(std::vector<HWElement *> & children) {
    return HWElement::Gate(BOR, children);
}


HWElement & xorGate(std::vector<HWElement *> & children) {
    return HWElement::Gate(BXOR, children);
}


HWElement & andGate(HWElement & child0, HWElement & child1) {
    std::vector<HWElement *> children {&child0, &child1};
    return HWElement::Gate(BAND, children);
}


HWElement & orGate(HWElement & child0, HWElement & child1) {
    std::vector<HWElement *> children {&child0, &child1};
    return HWElement::Gate(BOR, children);
}


HWElement & xorGate(HWElement & child0, HWElement & child1) {
    std::vector<HWElement *> children {&child0, &child1};
    return HWElement::Gate(BXOR, children);
}


HWElement & notGate(HWElement & child) {
    std::vector<HWElement *> children {&child};
    return HWElement::Gate(BNOT, children);
}



// Useless but for homogeneity
HWElement & inputGate(Node & child) {
    return HWElement::InputGate(child);
}

HWElement & Register(HWElement & child) {
    return HWElement::Register(child);
}


void getReachableGates(HWElement * g, std::set<HWElement *> & rg) {
    if (!rg.contains(g)) {
        rg.insert(g);
    }
    for (const auto & child : g->inputs) {
        getReachableGates(child, rg);
    }
}


void dumpCircuit(const char * filename, std::vector<HWElement *> & outputs) {
    std::set<HWElement *> reachableGates;
    for (const auto & gate : outputs) {
        getReachableGates(gate, reachableGates);
    }
    dumpCircuitGates(filename, reachableGates);
}


int32_t checkSecurity(int32_t order, bool withGlitches, SecurityProperty secProp, std::vector<HWElement *> & outputs) {
    int32_t nbCheck;
    return checkSecurity(order, withGlitches, secProp, outputs, &nbCheck);
}


int32_t checkSecurity(int32_t order, bool withGlitches, SecurityProperty secProp, std::vector<HWElement *> & outputs, int32_t * nbCheck) {
    std::cout << "# Checking Security at order " << order << " (" << (withGlitches ? "with glitches" : "no glitches") << ", " << secProp2str(secProp) << " property)" << std::endl;

    auto tupleEnum = [withGlitches](std::vector<HWElement *> & gateList, int32_t order, bool includePartialTuples, std::set<std::tuple<std::vector<Node *> *, std::vector<HWElement *> * >> & tuples) {
        const int32_t tupleLen = order;
        HWElement ** t = new HWElement * [tupleLen];

        std::function<void(int32_t i, int32_t nbTaken, bool includePartialTuples, std::set<std::tuple<std::vector<Node *> *, std::vector<HWElement *> * >> & tuples)> tupleEnumRec;
        tupleEnumRec = [&tupleEnumRec, &gateList, withGlitches, tupleLen, &t](int32_t i, int32_t nbTaken, bool includePartialTuples, std::set<std::tuple<std::vector<Node *> *, std::vector<HWElement *> * >> & tuples) {
            auto getLeakExps = [withGlitches](HWElement ** gates, int32_t nbTaken, std::set<Node *> & s) {
                for (int32_t i = 0; i < nbTaken; i += 1) {
                    HWElement * gate = gates[i];
                    if (withGlitches) {
                        for (const auto & leakExp : gate->leakageOut) {
                            s.insert(leakExp);
                        }
                    }
                    else {
                        s.insert(gate->symbExp);
                    }
                }
            };
    
            if (nbTaken == tupleLen || (includePartialTuples && nbTaken > 0)) {
                std::set<Node *> s;
                getLeakExps(t, nbTaken, s);
                std::vector<Node *> * v = new std::vector<Node *>();
                for (const auto & n : s) {
                    v->push_back(n);
                }
                std::vector<HWElement *> * w = new std::vector<HWElement *>();
                for (int32_t i = 0; i < nbTaken; i += 1) {
                    w->push_back(t[i]);
                }
                std::tuple<std::vector<Node *> *, std::vector<HWElement *> *> tup {v, w};
                tuples.insert(tup);
                if (nbTaken == tupleLen) {
                    return;
                }
            }

            /*
            if (includePartialTuples && nbTaken > 0) {
                set::set<Node *> s;
                getLeakExps(t, nbTaken, s);
                std::vector<Node *> * v = new std::vector<Node *>();
                for (const auto & n : s) {
                    v->push_back(n);
                }
                std::vector<HWElement *> * w = new std::vector<HWElement *>();
                for (int32_t i = 0; i < nbTaken; i += 1) {
                    w->push_back(t[i]);
                }
                std::tuple<std::vector<Node *>, std::vector<HWElement *>> tup {v, w}

                tuples.insert(tup);
            }
            */

            for (int32_t idx = i; idx < (int32_t) gateList.size(); idx += 1) {
                t[nbTaken] = gateList[idx];
                tupleEnumRec(idx + 1, nbTaken + 1, includePartialTuples, tuples);
            }
        };

        tupleEnumRec(0, 0, includePartialTuples, tuples);

        delete [] t;
    };

    auto vecGatesNumStr = [](std::vector<HWElement *> & t) {
        std::ostringstream ss;
        for (bool first{true}; const auto & g : t) {
            ss << (first ? first = false, "" : ", ") << g->num;
        }
        return ss.str();
    };

    auto setGatesNumStr = [](std::set<HWElement *> & t) {
        std::ostringstream ss;
        for (bool first{true}; const auto & g : t) {
            ss << (first ? first = false, "" : ", ") << g->num;
        }
        return ss.str();
    };

    auto vecExpsStr = [](std::vector<Node *> & t) {
        std::ostringstream ss;
        for (bool first{true}; const auto & e : t) {
            ss << (first ? first = false, "" : ", ") << e->toString();
        }
        return ss.str();
    };

    auto setExpsStr = [](std::set<Node *> & t) {
        std::ostringstream ss;
        for (bool first{true}; const auto & e : t) {
            ss << (first ? first = false, "" : ", ") << e->toString();
        }
        return ss.str();
    };



    std::set<HWElement *> reachableGates;
    for (const auto & gate : outputs) {
        getReachableGates(gate, reachableGates);
    }

    std::cout << "# Reachable gates (" << reachableGates.size() << "): " << setGatesNumStr(reachableGates) << std::endl;

    bool doRemSingleInputProbesOpt;
    std::set<HWElement *> reducedGates;
    if (withGlitches) {
        doRemSingleInputProbesOpt = false;
        // Removing components with redudant exps
        if (false) {
            reducedGates = reachableGates;
        }
        else {
            for (const auto & gate : reachableGates) {
                bool isSubset = false;
                std::set<HWElement *> toRemove;
                for (const auto & g : reducedGates) {
                    //if (g->leakageOut.issubset(gate->leakageOut)) 
                    if (std::includes(gate->leakageOut.begin(), gate->leakageOut.end(), g->leakageOut.begin(), g->leakageOut.end())) {
                        toRemove.insert(g);
                    }
                    //else if (gate->leakageOut.issubset(g->leakageOut))
                    if (std::includes(g->leakageOut.begin(), g->leakageOut.end(), gate->leakageOut.begin(), gate->leakageOut.end())) {
                        isSubset = true;
                        break;
                    }
                }
                if (!isSubset) {
                    reducedGates.insert(gate);
                }
                for (const auto & g : toRemove) {
                    reducedGates.erase(g);
                }
            }
        }
    }

    else if (secProp == NI || secProp == RNI || secProp == SNI || secProp == PINI) {
        // Tuple reduction is not applicable to TPS
        // Checking if all input shares are part of the reachable gates
        std::set<Node *> reachableInputShares;
        for (const auto & gate : reachableGates) {
            if (gate->nature == GATE && gate->op == INPUT && gate->symbExp->nature == SYMB && gate->symbExp->symbType == 'A') {
                reachableInputShares.insert(gate->symbExp);
            }
        }
        
        //print(' '.join(map(lambda x: '%s' % x.symbExp.symb, reachableInputShares)))
        std::cout << "# Reachable Input Shares: ";
        for (const auto & n : reachableInputShares) {
            std::cout << *n->symb << " ";
        }
        std::cout << std::endl;

        std::set<std::tuple<Node *, int32_t>> reachableInputSharesSecrets;
        for (const auto & n : reachableInputShares) {
            reachableInputSharesSecrets.insert({n->origSecret, n->nbShares});
        }
        //print(' '.join(map(lambda x: '%s' % x[0].symb, reachableInputSharesSecrets)))
        std::cout << "# Reachable Input Shares Secrets: ";
        for (const auto & tup : reachableInputSharesSecrets) {
            std::cout << *std::get<0>(tup)->symb << " ";
        }
        std::cout << std::endl;

        bool allInputShares = true;
        for (const auto & t : reachableInputSharesSecrets) {
            int32_t numShares = 0; // Number of accessible different input shares corresponding to the secret
            for (const auto & n : reachableInputShares) {
                if (n->origSecret == std::get<0>(t)) {
                    numShares += 1;
                }
            }
            if (numShares != std::get<1>(t)) {
                allInputShares = false;
                break;
            }
        }


        reducedGates = reachableGates;
        std::set<HWElement *> withdrawnGates;
        std::vector<HWElement *> reachableGatesVec(reachableGates.begin(), reachableGates.end());
        std::sort(reachableGatesVec.begin(), reachableGatesVec.end(), [](HWElement * a, HWElement * b) {
                return a->num < b->num;
        });

        // Remove the gate / probe if it contains at most one share per input and no random
        doRemSingleInputProbesOpt = HWElement::remSingleInputProbesOpt && allInputShares;
        if (doRemSingleInputProbesOpt) {
            std::cout << "# Removing Probes with at most 1 share / input and no random" << std::endl;
            for (const auto & g : reachableGatesVec) {
                bool verifyGate = true;
                bool moreThanOneOcc = false;
                for (const auto & [secret, val] : *g->symbExp->shareOcc) {
                    if (val->size() > 1) {
                        moreThanOneOcc = true;
                        break;
                    }
                }
                
                if (!moreThanOneOcc) {
                    if (g->symbExp->maskingMaskOcc->size() + g->symbExp->otherMaskOcc->size() == 0) {
                        verifyGate = false;
                    }
                }

                if (!verifyGate) {
                    std::cout << "# Removing gate " << g->num << ": " << *g->symbExp << std::endl;
                    reducedGates.erase(g);
                    withdrawnGates.insert(g);
                }
            }
        }


        // Remove the gate / probe if it has exactly the same masking randoms and the same or a subset of the input shares of another gate
        bool doRemRedundantProbesOpt = HWElement::remRedundantProbesOpt;
        if (doRemRedundantProbesOpt) {
            std::cout << "# Removing Redundant Probes" << std::endl;
            std::vector<HWElement *> reducedGatesVec(reducedGates.begin(), reducedGates.end());
            std::sort(reducedGatesVec.begin(), reducedGatesVec.end(), [](HWElement * a, HWElement * b) {
                return a->num < b->num;
            });


            for (const auto & g : reducedGatesVec) {
                bool verifyGate = true;
                if (g->symbExp->otherMaskOcc->size() == 0 || (g->symbExp->nature == SYMB && g->symbExp->symbType == 'M')) {
                    // FIXME: why reachable and not reduced ? this should allow to remove the second if,
                    // which should always be false in that case...
                    for (const auto & h : reachableGates) {
                        if (g == h) {
                            continue;
                        }
                        if (withdrawnGates.contains(h)) {
                            continue;
                        }
                        std::set<Node *> gShares;
                        for (const auto & [secret, val] : *g->symbExp->shareOcc) {
                            for (const auto & [sh, num] : *val) {
                                gShares.insert(sh);
                            }
                        }
                        std::set<Node *> hShares;
                        for (const auto & [secret, val] : *h->symbExp->shareOcc) {
                            for (const auto & [sh, num] : *val) {
                                hShares.insert(sh);
                            }
                        }

                        // FIXME? For SNI and PINI, shouldn't we prevent the case in which the larger gate is an internal gate and the redundant gate an output gate?
                        if (h->symbExp->otherMaskOcc->size() == 0) {
                            std::set<Node *> gMasks;
                            std::set<Node *> hMasks;
                            std::transform(g->symbExp->maskingMaskOcc->begin(), g->symbExp->maskingMaskOcc->end(), std::inserter(gMasks, gMasks.end()), [](auto pair){ return pair.first; });
                            std::transform(h->symbExp->maskingMaskOcc->begin(), h->symbExp->maskingMaskOcc->end(), std::inserter(hMasks, hMasks.end()), [](auto pair){ return pair.first; });
                            if (gMasks == hMasks && std::includes(hShares.begin(), hShares.end(), gShares.begin(), gShares.end())) {
                                //std::cout << "# Shares of g: " << setExpsStr(gShares) << std::endl;
                                //std::cout << "# Shares of h: " << setExpsStr(hShares) << std::endl;
                                //std::cout << "# Larger Gate: " << h->num << ": " << *h->symbExp << std::endl;
                                verifyGate = false;
                                break;
                            }
                            else if (g->symbExp->nature == SYMB && g->symbExp->symbType == 'M' && h->symbExp->maskingMaskOcc->size() == 1 && h->symbExp->maskingMaskOcc->count(g->symbExp) == 1) {
                                //if (set([g.symbExp]) == set(h.symbExp.maskingMaskOcc.keys()))
                                verifyGate = false;
                                //std::cout << "# Larger Gate for " << *g->symbExp << ": " << h->num << ": " << *h->symbExp << std::endl;
                                break;
                            }
                        }
                    }
                }

                if (!verifyGate) {
                    std::cout << "# Removing gate " << g->num << ": " << *g->symbExp << std::endl;
                    reducedGates.erase(g);
                    withdrawnGates.insert(g);
                }

            } // g
        } // doRemRedundantProbesOpt
    } // secProp

    else {
        assert(secProp == TPS);
        reducedGates = reachableGates;
        doRemSingleInputProbesOpt = false;
    }

    std::vector<HWElement *> gates(reducedGates.begin(), reducedGates.end());
    std::sort(gates.begin(), gates.end(), [](HWElement * a, HWElement * b) {
        return a->num < b->num;
    });


    std::cout << "# Reduced gates (" << gates.size() << "):   " << vecGatesNumStr(gates) << std::endl;
    for (const auto & gate : gates) {
        std::cout << "# Gate " << gate->num << ": " << *gate->symbExp << std::endl;
    }


    if ((secProp == NI || secProp == TPS) && HWElement::bartheOpt) {
        std::map<Node *, std::string> expNames;
        int32_t expNum = 0;
        std::set<Node *> reducedGatesExp;
        for (const auto & g : reducedGates) {
            if (withGlitches) {
                for (const auto & leakExp : g->leakageOut) {
                    reducedGatesExp.insert(leakExp);
                }
            }
            else {
                reducedGatesExp.insert(g->symbExp);
                expNames.insert_or_assign(g->symbExp, std::string("e") + std::to_string(expNum));
                expNum += 1;
            }
        }
 
        auto checkProp = [secProp](Node & e, int32_t order) {
            if (secProp == TPS) {
                return checkTpsVal(e);
            }
            else {
                return checkNIVal(e, order);
            }
        };


        auto choose = [](std::set<Node *> & e, int32_t nb, std::set<Node *> & s) {
            int32_t cnt = 0;
            for (const auto & elem : e) {
                s.insert(elem);
                cnt += 1;
                if (cnt == nb) {
                    break;
                }
            }
        };

        auto chooseSingle = [](std::set<Node *> & e) -> Node & {
            for (const auto & elem : e) {
                return *elem;
            }
            assert(false);
        };


        /* Modifies y and e */
        std::function<void(std::set<Node *> & y, std::set<Node *> & e)> extend;
        extend = [&chooseSingle, &checkProp, order, &extend](std::set<Node *> & y, std::set<Node *> & e) {
            if (e.size() == 0) {
                return;
            }
            Node & elem = chooseSingle(e);
            y.insert(&elem);
            std::vector<Node *> yVec(y.begin(), y.end());
            Node & conc = Concat(yVec);
            bool res = checkProp(conc, order);
            HWElement::nbNIcalls += 1;
            e.erase(&elem);
            if (res) {
                extend(y, e);
            }
            else {
                y.erase(&elem);
                extend(y, e);
            }
        };

        auto tupleEnumBis = [](std::set<Node *> & expsSet, int32_t order, std::vector<std::vector<Node *> *> & tuples) {
            // same idea as tupleEnum but with the following differences:
            // - it does not enumerates on gates, but directly on symb exps
            // - there is no partial tuples
            std::vector<Node *> expsVec(expsSet.begin(), expsSet.end());
            Node ** t = new Node * [order];

            std::function<void(int32_t i, int32_t nbTaken)> tupleEnumBisRec;
            tupleEnumBisRec = [order, t, &tuples, &expsVec, &tupleEnumBisRec](int32_t i, int32_t nbTaken) {
                if (nbTaken == order) {
                    //std::vector<Node *> * v = new std::vector<Node *>();
                    //for (int32_t j = 0; j < order; j += 1) {
                    //    v->push_back(t[i]);
                    //}
                    //std::unique_ptr<std::vector<Node *>> v(new std::vector<Node *>(t, t + order));
                    std::vector<Node *> * v = new std::vector<Node *>(t, t + order); // TODO: do not forget to delete
                    tuples.push_back(v);
                    return;
                }

                for (int32_t idx = i; idx < (int32_t) expsVec.size(); idx += 1) {
                    t[nbTaken] = expsVec[idx];
                    tupleEnumBisRec(idx + 1, nbTaken + 1);
                }
            };

            tupleEnumBisRec(0, 0);
            delete [] t;
        };

        std::function<bool(std::set<Node *> & x, int32_t d, std::set<Node *> & e, int32_t depth)> check;
        check = [&check, &expNames, &extend, &choose, &checkProp, &tupleEnumBis, order](std::set<Node *> & x, int32_t d, std::set<Node *> & e, int32_t depth) {
            auto localPrint = [depth](std::string param) {
                std::cout << "# ";
                for (int32_t i = 0; i < depth; i += 1) {
                    std::cout << "   ";
                }
                std::cout << param << std::endl;
            };

            auto printExps = [&localPrint, &expNames](std::string str, std::set<Node *> & x) {
                std::string s0 = str;
                s0 += " = [";
                for (bool first{true}; const auto & u : x) {
                    s0 += (first ? first = false, "" : ", ") + expNames.at(u);
                }
                s0 += "]";
                localPrint(s0);
                //s0 = "";
                //for (int32_t i = 0; i < (int32_t) str.size(); i += 1) {
                //    s0 += " ";
                //}
                //s0 += " = [";
                //for (bool first{true}; const auto & u : x) {
                //    s0 += (first ? first = false, "" : ", ") + u->toString();
                //}
                //s0 += "]";
                //localPrint(s0);
            };


            localPrint(std::string("check (d = ") + std::to_string(d) + ")");
            printExps("x", x);
            printExps("e", e);

            assert((int32_t) x.size() + d == order);
            if (d <= (int32_t) e.size()) {
                std::set<Node *> y;
                choose(e, d, y);
                printExps("y", y);
                std::set<Node *> z(x);
                z.insert(y.begin(), y.end());
                printExps("(x, y)", z);
                std::vector<Node *> zVec(z.begin(), z.end());
                Node & conc = Concat(zVec);
                bool h = checkProp(conc, order);
                HWElement::nbNIcalls += 1;
                if (!h) {
                    localPrint("# checkProp(x, y) returned false");
                    return false;
                }
                
                std::set<Node *> eMinusy;
                std::set_difference(e.begin(), e.end(), y.begin(), y.end(), std::inserter(eMinusy, eMinusy.end()));
                // Here z is extended, i.e. z is yc
                // yc = extend(z, e - y);
                printExps("eMinusy", eMinusy);
                extend(z, eMinusy);
                std::set<Node *> & yc = z;
                localPrint("calling extend");
                printExps("yc", yc);
                std::set<Node *> & eMinusyc = eMinusy;
                // eMinusyc = e - yc;
                std::set_difference(e.begin(), e.end(), yc.begin(), yc.end(), std::inserter(eMinusyc, eMinusyc.end()));
                printExps("eMinusyc", eMinusyc);
                std::set<Node *> ycMinusx;
                //ycMinusx = yc - x;
                std::set_difference(yc.begin(), yc.end(), x.begin(), x.end(), std::inserter(ycMinusx, ycMinusx.end()));
                printExps("ycMinusx", ycMinusx);
                bool res = check(x, d, eMinusyc, depth + 1);
                if (!res) {
                    localPrint("# Not res (main check), return false");
                    return false;
                }
                std::vector<std::vector<Node *> *> tuples;
                for (int32_t i = 1; i < d; i += 1) {
                    tupleEnumBis(ycMinusx, i, tuples);
                    for (const auto & t : tuples) {
                        std::set<Node *> xUniont(x);
                        xUniont.insert(t->begin(), t->end());
                        bool res = check(xUniont, d - i, eMinusyc, depth + 1);
                        if (!res) {
                            localPrint("# Not res (check in for loop), return false");
                            return false;
                        }
                    }
                    while (tuples.size() != 0) {
                        std::vector<Node *> * v = tuples.back();
                        delete v;
                        tuples.pop_back();
                    }
                }
                return true;
            }
            else {
                return true;
            }
        };

        std::cout << "# reducedGatesExp:" << std::endl;
        for (const auto & exp : reducedGatesExp) {
            std::cout << "#   Exp: " << exp->toString() << std::endl;
        }
        std::set<Node *> s;
        bool res = check(s, order, reducedGatesExp, 0);
        std::cout << "# Res for Barthe Algo: " << res << std::endl;
        std::cout << "# Nb. NI calls: " <<  HWElement::nbNIcalls << std::endl;
        *nbCheck = HWElement::nbNIcalls;
        if (res) {
            return 0;
        }
        else {
            return 1;
        }
    }
    else {

        std::cout << "# Starting tuple enumeration" << std::endl;
        std::set<std::tuple<std::vector<Node *> *, std::vector<HWElement *> * >> tuples;
        tupleEnum(gates, order, doRemSingleInputProbesOpt, tuples);
        std::cout << "# Number of tuples: " << tuples.size() << std::endl;
        //for t in tuples:
        //    print('# (' + ', '.join(map(lambda x: '%d' % x.num, sorted(t[1], key = lambda x: x.num))) + ')')

        std::vector<std::tuple<std::vector<Node *> *, std::vector<HWElement *> *>> leakingHwe;
        for (const auto & t : tuples) {
            //std::cout << "# Checking expression for component(s) (" << vecGatesNumStr(*std::get<1>(t)) << "): " << vecExpsStr(*std::get<0>(t)) << std::endl;

            bool res;
            if (secProp == TPS) {
                Node & conc = Concat(*std::get<0>(t));
                res = checkTpsVal(conc);
                //delete &conc;
            }
            else if (secProp == NI) {
                //std::cout << "# checking NI for exps " << vecExpsStr(*std::get<0>(t)) << " and maxShareOcc = " << std::get<1>(t)->size() << std::endl;
                //%s and maxShareOcc = %d' % (', '.join(map(lambda x: '%s' % x, t[0])), len(t[1])));
                Node & conc = Concat(*std::get<0>(t));
                res = checkNIVal(conc, std::get<1>(t)->size());
                //delete &conc;
            }
            else if (secProp == RNI) {
                //std::cout << "# checking RNI for exps " << vecExpsStr(*std::get<0>(t)) << " and maxShareOcc = (nbShares - 1) - " << (order - std::get<1>(t)->size()) << std::endl;
                //%s and maxShareOcc = (nbShares - 1) - %d' % (', '.join(map(lambda x: '%s' % x, t[0])), (order - len(t[1]))))
                Node & conc = Concat(*std::get<0>(t));
                res = checkRNIVal(conc, (order - std::get<1>(t)->size()));
                //delete &conc;
            }
            else if (secProp == SNI) {
                int32_t nbOutputProbes = 0;
                for (const auto & probe : *std::get<1>(t)) {
                    if (std::find(outputs.begin(), outputs.end(), probe) != outputs.end()) {
                        nbOutputProbes += 1;
                    }
                }
                Node & conc = Concat(*std::get<0>(t));
                res = checkNIVal(conc, std::get<1>(t)->size() - nbOutputProbes);
                //delete &conc;
            }
            else if (secProp == PINI) {
                std::set<int> outputIndexes;
                for (const auto & probe : *std::get<1>(t)) {
                    for (int32_t i = 0; i < (int32_t) outputs.size(); i += 1) {
                        if (probe == outputs.at(i)) {
                            outputIndexes.insert(i);
                        }
                    }
                }
                Node & conc = Concat(*std::get<0>(t));
                res = checkPINIVal(conc, (std::get<1>(t)->size() - outputIndexes.size()), outputIndexes);
                //delete &conc;
            }
            else {
                assert(false);
            }

            if (!res) {
                std::cout << "# Leaking expression for component(s) (" << vecGatesNumStr(*std::get<1>(t)) << "): " << vecExpsStr(*std::get<0>(t)) << std::endl;
                leakingHwe.push_back(t);
            }
            else {
                delete std::get<0>(t);
                delete std::get<1>(t);
            }
        }

        int32_t res = leakingHwe.size();
        if (leakingHwe.size() != 0) {
            std::cout <<  "# Following Components\' outputs are not " << secProp2str(secProp) << " secure at order " << order << " " << (withGlitches ? "with" : "without") << " glitches:" << std::endl;
            for (const auto & hwe : leakingHwe) {
                std::cout << "# Leaking expression for component(s) (" << vecGatesNumStr(*std::get<1>(hwe)) << "): " << vecExpsStr(*std::get<0>(hwe)) << std::endl;
                delete std::get<0>(hwe);
                delete std::get<1>(hwe);
            }
        }
        else {
            std::cout << "# Circuit is secure in the " << secProp2str(secProp) << " security model at order " << order << " " << (withGlitches ? "with" : "without") << " glitches" << std::endl;
        }

        *nbCheck = tuples.size();
        return res;
    }
}

