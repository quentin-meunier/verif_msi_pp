/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#ifndef _arrayexp_hpp_
#define _arrayexp_hpp_

#include <functional>

#include "node.hpp"


namespace vmsi {

// Category of an array, computed at construction (see README, section Arrays).
// Only permutations (declared or checked on the content) propagate masks:
// T[m] is then a uniform value over outWidth bits and can mask other values.
enum ArrayKind {
    ARRAY_FUNCTION,             // func provided: accesses are replaced by func(index), no ARRAY node
    ARRAY_KNOWN_PERMUTATION,    // content known, full domain, inWidth == outWidth, values all distinct
    ARRAY_KNOWN_FUNCTION,       // content known, not a permutation
    ARRAY_DECLARED_PERMUTATION, // content unknown, declared as a permutation by the user
    ARRAY_OPAQUE_FUNCTION,      // content unknown, not declared as a permutation
};

class ArrayExp {

    public:
    std::string name;
    int32_t inWidth;
    int32_t outWidth;
    uint64_t addr;
    int32_t size;
    std::function<Node &(Node &)> func;
    void * content;
    int32_t elemSize; // size in bytes of an element in the concrete array
    ArrayKind kind;
    bool maskPreserving; // true iff an access to the array preserves masking (permutation)

    inline static std::map<std::string, ArrayExp *> allArrays;
    inline static std::set<ArrayExp *> registeredArrays;
    inline static std::map<std::string, ArrayExp *> registeredArraysByName;
    inline static std::map<uint64_t, ArrayExp *> registeredArraysByAddr;

    ArrayExp(const std::string & name, int32_t inWidth, int32_t outWidth, bool isPermutation = false);
    ArrayExp(const std::string & name, int32_t inWidth, int32_t outWidth, uint64_t addr, int32_t size, std::function<Node &(Node &)> func, void * content, int32_t elemSize, bool isPermutation = false);
    ~ArrayExp();
 
    static void cleanup();
    static ArrayExp & getByName(const std::string & name);

    Node & operator[](Node & child);
    uint64_t getContent(uint64_t idx);
    std::string kindStr() const;

    private:
    void classify(bool isPermutation);

};

} // namespace vmsi


#endif

