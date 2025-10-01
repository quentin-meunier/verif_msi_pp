/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/

#include <set>
#include <cassert>

#include "check_leakage.hpp"
#include "node.hpp"
#include "utils.hpp"
#include "tps.hpp"
#include "simplify.hpp"




#define CHECK_VAL_BODY(check_call, check_call_bd) ({    \
    bool res;                                           \
    bool usedBitExp;                                    \
                                                        \
    if (e.hasWordOp) {                                  \
        res = false;                                    \
        usedBitExp = false;                             \
        if (!e.wordAnalysisHasFailedOnSubExp) {         \
            res = check_call;                           \
            if (!res) {                                 \
                e.wordAnalysisHasFailedOnSubExp = true; \
            }                                           \
        }                                               \
                                                        \
        if (!res && bitExpEnable()) {                   \
            usedBitExp = true;                          \
            res = check_call_bd;                        \
        }                                               \
    }                                                   \
    else {                                              \
        if (bitExpEnable()) {                           \
            usedBitExp = true;                          \
            res = check_call_bd;                        \
        }                                               \
        else {                                          \
            usedBitExp = false;                         \
            res = check_call;                           \
        }                                               \
    }                                                   \
                                                        \
    if (usedBitExpRet != NULL) {                        \
        *usedBitExpRet = usedBitExp;                    \
    }                                                   \
    return res;                                         \
})


bool checkTpsVal(Node & e, bool * usedBitExpRet) {
    CHECK_VAL_BODY(tps(e), tps(e, true));
}

bool checkNIVal(Node & e, int maxShareOcc, bool * usedBitExpRet) {
    CHECK_VAL_BODY(ni(e, maxShareOcc), ni(e, maxShareOcc, true));
}

bool checkRNIVal(Node & e, int diff, bool * usedBitExpRet) {
    CHECK_VAL_BODY(rni(e, diff), rni(e, diff, true));
}

bool checkPINIVal(Node & e, int maxShareOcc, std::set<int> & outputIndexes, bool * usedBitExpRet) {
    CHECK_VAL_BODY(pini(e, maxShareOcc, outputIndexes), pini(e, maxShareOcc, outputIndexes, true));
}

#undef CHECK_VAL_BODY

#define CHECK_VAL_BODY_BIT(check_call) ({                      \
    bool res;                                                  \
                                                               \
    for (int32_t i = 0; i < e.width; i += 1) {                 \
        Node & b = simplifyUSBV(Extract(i, i, e));             \
        res = check_call;                                      \
        if (!res) {                                            \
            break;                                             \
        }                                                      \
    }                                                          \
                                                               \
    return res;                                                \
})


bool checkTpsValBit(Node & e) {
    CHECK_VAL_BODY_BIT(tps(b));
}

bool checkNIValBit(Node & e, int maxShareOcc) {
    CHECK_VAL_BODY_BIT(ni(b, maxShareOcc));
}

bool checkRNIValBit(Node & e, int diff) {
    CHECK_VAL_BODY_BIT(rni(b, diff));
}

bool checkPINIValBit(Node & e, int maxShareOcc, std::set<int> & outputIndexes) {
    CHECK_VAL_BODY_BIT(pini(b, maxShareOcc, outputIndexes));
}

#undef CHECK_VAL_BODY_BIT



#define CHECK_TRANS_BODY(check_call, check_call_bd) ({                                 \
    bool res;                                                                          \
    bool usedBitExp;                                                                   \
                                                                                       \
    std::vector<Node *> n = {&e0, &e1};                                                \
                                                                                       \
    if (e0.hasWordOp || e1.hasWordOp) {                                                \
        res = false;                                                                   \
        usedBitExp = false;                                                            \
        if (!(e0.wordAnalysisHasFailedOnSubExp || e1.wordAnalysisHasFailedOnSubExp)) { \
            res = check_call;                                                          \
            /* FIXME: if only transition and no value, how to make the flag            \
               become true? check each exp independently?                              \
               if (!res) {                                                             \
                  e0.wordAnalysisHasFailedOnSubExp = true;                             \
                  e1.wordAnalysisHasFailedOnSubExp = true;                             \
               }                                                                       \
            */                                                                         \
        }                                                                              \
                                                                                       \
        if (!res && bitExpEnable()) {                                                  \
            usedBitExp = true;                                                         \
            res = check_call_bd;                                                       \
        }                                                                              \
    }                                                                                  \
                                                                                       \
    else {                                                                             \
        if (bitExpEnable()) {                                                          \
            usedBitExp = true;                                                         \
            res = check_call_bd;                                                       \
        }                                                                              \
        else {                                                                         \
            usedBitExp = false;                                                        \
            res = check_call;                                                          \
        }                                                                              \
    }                                                                                  \
                                                                                       \
    if (usedBitExpRet != NULL) {                                                       \
        *usedBitExpRet = usedBitExp;                                                   \
    }                                                                                  \
    return res;                                                                        \
})


bool checkTpsTrans(Node & e0, Node & e1, bool * usedBitExpRet) {
    CHECK_TRANS_BODY(tps(n), tps(n, true));
}

bool checkNITrans(Node & e0, Node & e1, int maxShareOcc, bool * usedBitExpRet) {
    CHECK_TRANS_BODY(ni(n, maxShareOcc), ni(n, maxShareOcc, true));
}

bool checkRNITrans(Node & e0, Node & e1, int diff, bool * usedBitExpRet) {
    CHECK_TRANS_BODY(rni(n, diff), rni(n, diff, true));
}

bool checkPINITrans(Node & e0, Node & e1, int maxShareOcc, std::set<int> & outputIndexes, bool * usedBitExpRet) {
    CHECK_TRANS_BODY(pini(n, maxShareOcc, outputIndexes), pini(n, maxShareOcc, outputIndexes, true));
}


#undef CHECK_TRANS_BODY


