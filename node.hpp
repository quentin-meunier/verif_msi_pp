/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#ifndef _node_hpp_
#define _node_hpp_

#include <limits>
#include <bit>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <ostream>

#include "config.hpp"



typedef enum _NodeNature {
    NONE = 0,
    SYMB,
    CONST,
    STR,
    OP,
} NodeNature;


typedef enum _NodeOp {
    NOP = 0,
    BXOR,
    BAND,
    BOR,
    BNOT,
    LSHR,
    LSHL,
    ASHR,
    SLSHR,
    SLSHL,
    SASHR,
    PLUS,
    MINUS,
    UMINUS,
    IMUL,
    IPOW,
    GMUL,
    GPOW,
    GLOG,
    GEXP,
    ZEXT,
    SEXT,
    CONCAT,
    EXTRACT,
    ARRAY,
    INPUT,
} NodeOp;



static inline bool propagateCstOnBuild() {
    return PROPAGATE_CST_ON_BUILD;
}


template <class T>
constexpr int bit_width(T x) {
    assert(std::numeric_limits<T>::digits == 64);
    return (x == 0 ? 1 : 64 - __builtin_clzll(x));
    //return std::numeric_limits<T>::digits;
    //return std::bit_width(x);
}


class Node {

    public:
    inline static const std::set<NodeOp> associativeOps = {BXOR, PLUS, BAND, BOR, IMUL, GMUL};
    inline static const std::set<NodeOp> bitwiseOps = {BXOR, BAND, BOR, BNOT};
    inline static const std::set<NodeOp> maskingOps = {BXOR, PLUS};
    inline static const std::set<NodeOp> preserveMaskingOps = {BNOT, ARRAY, UMINUS};
    inline static const std::set<char> forbiddenChars = {'#', '@', '[', ']'};
    
    inline static std::map<std::string, Node *> symb2node;
    inline static std::map<int32_t, std::map<uint64_t, Node *>> cst2node;
    inline static std::map<std::tuple<uint64_t, uint64_t, uint64_t, uint64_t>, Node *> bigCst2node;
    inline static std::map<std::string, Node *> str2node;
   
    inline static int32_t nodeNum = 0;
    inline static const int32_t gfBase = 3;
    inline static const int32_t gfIrr = 0x1b;
    inline static const std::set<int32_t> powersTwo = {1, 2, 4, 8, 16, 32, 64, 128};

    inline static bool modeTempNode = false;
    inline static std::set<Node *> toDelete;
    inline static std::map<std::tuple<uint64_t, uint64_t, uint64_t, uint64_t>, Node *> cache;
    inline static std::set<Node *> opNodes;

    int32_t num;
    int32_t width;
    std::vector<Node *> * children;
    int32_t nbShares;
    int32_t shareNum;
    std::string * symb;
    char symbType;
    NodeNature nature;
    NodeOp op;
    bool hasWordOp;
    bool wordAnalysisHasFailedOnSubExp;
    int32_t nlimbs;
    uint64_t * cst;
    std::string * strn;

    // Cache to Extract node for each bit
    std::vector<Node *> extractBit;
    // Cache for the concatenation of all the extracted bits, with single bit variables
    Node * concatExtEq;
    // simplified equivalent node
    Node * simpEq;
    // simplified equivalent node using single-bit variables
    Node * simpEqUsbv;
    Node * origSecret; // For share nodes: original secret from which the share is derived
    Node * pseudoShareEq; // Pseudo-share equivalent

    std::map<Node *, int32_t> * secretVarOcc;
    std::map<Node *, int32_t> * publicVarOcc;
    std::map<Node *, Node *> * currentlyMasking;
    std::map<Node *, std::map<Node *, std::map<Node *, std::pair<int32_t, int32_t>> * > * > * maskingMaskOcc;
    std::map<Node *, std::map<Node *, int32_t> * > * otherMaskOcc;
    std::map<Node *, std::map<Node *, int32_t> * > * shareOcc;
    std::pair<Node *, Node *> preservedMask;

    uint64_t * h;


    Node();
    //Node(const Node & n);
    ~Node();

    static Node & SymbNode(const std::string & symb, char symbType, int32_t width, int32_t nbShares, int32_t shareNum, Node * origSecret, Node * pseudoShareEq);
    static Node & ConstNode(uint64_t cst, int32_t width);
    static Node & ConstNode(uint64_t * cst, int32_t nlimbs, int32_t width);
    static Node & ConstNodeAuto(uint64_t cst);
    static Node & StrNode(const std::string & s);
    static Node & OpNode(NodeOp op, const std::vector<Node *> & children);
    static const char * op2strOp(NodeOp op);

    static void setModeTemporaryNodes();
    static void setModePermanentNodes();

    void printVarOcc();
    void printMaskOcc();
    void setVarsOccurrences();

    
    private:
    void fillReachableNodes(Node * n, std::set<Node *> & rn);
    static void dumpNodes(const char * filename, std::set<Node *> & nodes);
    static Node & makeBitwiseNode(NodeOp op, Node * child0, Node * child1);

    public:
    void dump(const char * filename);
    Node & operator&(Node & other);
    Node & operator|(Node & other);
    Node & operator^(Node & other);
    Node & operator~();
    Node & operator+(Node & other);
    Node & operator-(Node & other);
    Node & operator-();
    Node & operator<<(Node & other);
    Node & operator<<(int32_t shval);
    Node & operator>>(Node & other);
    Node & operator>>(int32_t shval);
    Node & operator*(Node & other);

    friend std::ostream& operator<<(std::ostream &, const Node &);

    std::string toString() const;
    std::string verbatimPrint() const;
    std::string printCst() const;
    std::string expPrint(bool parNeeded, bool verbatim) const;

};


Node & Symb(const char * symb, char symbType, int32_t width, int32_t nbShares, int32_t shareNum, Node * origSecret, Node * pseudoShareEq);
Node & SymbInternal(std::string symb, char symbType, int32_t width, int32_t nbShares, int32_t shareNum, Node * origSecret, Node * pseudoShareEq);
Node & SymbInternal(std::string symb, char symbType, int32_t width);
Node & Const(uint64_t * cst, int32_t nlimbs, int32_t width);
Node & Const(uint64_t cst, int32_t width);
Node & Str(const std::string & s);
Node & LShR(Node & child, Node & shval);
Node & LShR(Node & child, int32_t shval);
Node & RotateRight(Node & child, Node & shval);
Node & RotateRight(Node & child, int32_t shval);
Node & ConstNodeFromConcat(const std::vector<Node *> & children);
Node & Concat(const std::vector<Node *> & children);
Node & Concat(Node & n0, Node & n1);
Node & Concat(Node & n0, Node & n1, Node & n2);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11, Node & n12);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11, Node & n12, Node & n13);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11, Node & n12, Node & n13, Node & n14);
Node & Concat(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7, Node & n8, Node & n9, Node & n10, Node & n11, Node & n12, Node & n13, Node & n14, Node & n15);
//template <typename T> Node & Concat(T t);
//template<typename T, typename... Args> Node & Concat(T t, Args... args);
Node & ConstNodeFromExtract(int32_t msb, int32_t lsb, const Node & n);
Node & Extract(Node & msb, Node & lsb, Node & child);
Node & Extract(int32_t msb, int32_t lsb, Node & child);
Node & ConstNodeFromZeroExt(int32_t numZeros, Node & n);
Node & ZeroExt(Node & numZeros, Node & child);
Node & ZeroExt(int32_t numZeros, Node & child);
Node & SignExt(Node & numSignBits, Node & child);
Node & SignExt(int32_t numSignBits, Node & child);
Node & GMul(std::vector<Node *> children);
Node & GMul(Node & n0, Node & n1);
Node & GMul(Node & n0, Node & n1, Node & n2);
Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3);
Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4);
Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5);
Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6);
Node & GMul(Node & n0, Node & n1, Node & n2, Node & n3, Node & n4, Node & n5, Node & n6, Node & n7);
//template <typename T> Node & GMul(T t);
//template<typename T, typename... Args> Node & GMul(T t, Args... args);
Node & GPow(Node & c0, Node & c1);
Node & GLog(Node & child);
Node & GExp(Node & child);
Node & imul(Node & n0, Node & n1);
uint64_t imulInt(uint64_t a, uint64_t b, int32_t w);
Node & ipow(Node & n0, Node & n1);
uint64_t ipowInt(uint64_t a, uint64_t b, int32_t w);
Node & gmul(Node & n0, Node & n1);
uint64_t gmulInt(uint64_t a, uint64_t b);
Node & gpow(Node & n0, Node & n1);
uint64_t gpowInt(uint64_t a, uint64_t b);
Node & gexp(Node & n);
uint64_t gexpInt(uint64_t a);
Node & glog(Node & n);
uint64_t glogInt(uint64_t a);

bool isZero(uint64_t * cst, int32_t width);
bool isAllOne(uint64_t * cst, int32_t width);


#endif


