/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#ifndef _hw_hpp_
#define _hw_hpp_

#include <set>
#include <vector>


#include "node.hpp"
#include "simplify.hpp"
#include "check_leakage.hpp"
#include "tps.hpp"
#include "config.hpp"
#include "utils.hpp"


typedef enum _HWElementNature {
    HWNONE,
    GATE,
    REGISTER,
} HWElementNature;


class HWElement {

    public:
    inline static int32_t nodeNum = 0;
    inline static int32_t nbNIcalls = 0;
    inline static const bool remProbesOpt = REM_SINGLE_INPUT_PROBES;
    inline static const bool remRedundantProbesOpt = REM_REDUNDANT_PROBES;
    inline static const bool bartheOpt = BARTHE_OPT;
    inline static std::set<HWElement *> allHWElements;

    int32_t num;
    HWElementNature nature;
    NodeOp op;
    std::vector<HWElement *> inputs;
    std::set<Node *> leakageOut;
    Node * symbExp;

    HWElement();
    ~HWElement();


    std::string toString() const;
    Node & getSymbExp() {
        return *symbExp;
    }

    static HWElement & InputGate(Node & input);
    static HWElement & Gate(NodeOp op, std::vector<HWElement *> & inputs);
    static HWElement & Register(HWElement & input);


    friend std::ostream& operator<<(std::ostream &, const HWElement &);

};


HWElement & andGate(HWElement & child0, HWElement & child1);
HWElement & orGate(HWElement & child0, HWElement & child1);
HWElement & xorGate(HWElement & child0, HWElement & child1);
HWElement & LShLGate(HWElement & child, int32_t shval);
HWElement & LShRGate(HWElement & child, int32_t shval);
HWElement & extractGate(int32_t msb, int32_t lsb, HWElement & child);
HWElement & notGate(HWElement & child);
HWElement & inputGate(Node & child);
HWElement & Register(HWElement & child);
HWElement & gmulGadget(HWElement & n0, HWElement & n1);

HWElement & andGate(std::vector<HWElement *> & children);
HWElement & orGate(std::vector<HWElement *> & children);
HWElement & xorGate(std::vector<HWElement *> & children);
HWElement & concatGate(std::vector<HWElement *> & children);


void dumpCircuit(const char * filename, std::vector<HWElement *> & outputs);

int32_t checkSecurity(int32_t order, bool withGlitches, SecurityProperty secProp, std::vector<HWElement *> & outputs, bool noFalsePositive = false, bool verbose = false);
int32_t checkSecurity(int32_t order, bool withGlitches, SecurityProperty secProp, std::vector<std::vector<HWElement *>> & outputList, bool noFalsePositive = false, bool verbose = false);
int32_t checkSecurity(int32_t order, bool withGlitches, SecurityProperty secProp, std::vector<HWElement *> & outputs, bool noFalsePositive, int32_t * nbCheck, bool verbose = false);
int32_t checkSecurity(int32_t order, bool withGlitches, SecurityProperty secProp, std::vector<std::vector<HWElement *>> & outputList, bool noFalsePositive, int32_t * nbCheck, bool verbose = false);



#endif

