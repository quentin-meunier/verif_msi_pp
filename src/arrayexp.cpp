/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#include <cassert>
#include <cstdlib>
#include <vector>

#include "arrayexp.hpp"
#include "node.hpp"


namespace vmsi {

ArrayExp::ArrayExp(const std::string & name, int32_t inWidth, int32_t outWidth, uint64_t addr, int32_t size, std::function<Node &(Node &)> func, void * content, int32_t elemSize, bool isPermutation) {
    this->name = name;
    this->inWidth = inWidth;
    this->outWidth = outWidth;
    this->addr = addr;
    this->size = size;
    this->func = func;
    this->content = content;
    this->elemSize = elemSize;

    if (allArrays.contains(name)) {
        std::cerr << "*** Error: ArrayExp name " << name << " already used" << std::endl;
        exit(EXIT_FAILURE);
    }

    classify(isPermutation);
    allArrays[name] = this;
}


ArrayExp::ArrayExp(const std::string & name, int32_t inWidth, int32_t outWidth, bool isPermutation) {
    this->name = name;
    this->inWidth = inWidth;
    this->outWidth = outWidth;
    this->addr = 0;
    this->size = 0;
    this->func = NULL;
    this->content = NULL;
    this->elemSize = 0;

    if (allArrays.contains(name)) {
        std::cerr << "*** Error: ArrayExp name " << name << " already used" << std::endl;
        exit(EXIT_FAILURE);
    }

    classify(isPermutation);
    allArrays[name] = this;
}


void ArrayExp::classify(bool isPermutation) {
    if (func != NULL) {
        // Accesses are replaced by func(index): no ARRAY node is ever created
        kind = ARRAY_FUNCTION;
        maskPreserving = false;
        return;
    }

    if (content != NULL) {
        if (elemSize != 1 and elemSize != 2 and elemSize != 4 and elemSize != 8) {
            std::cerr << "*** Error: Array " << name << ": element size must be 1, 2, 4 or 8 bytes (got " << elemSize << ")" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (size <= 0 or size % elemSize != 0) {
            std::cerr << "*** Error: Array " << name << ": size (" << size << " bytes) must be a positive multiple of the element size (" << elemSize << ")" << std::endl;
            exit(EXIT_FAILURE);
        }
        uint64_t nbElems = (uint64_t) (size / elemSize);
        // inWidth is at least 8 and size is an int32_t: 2^inWidth cannot overflow when compared
        bool fullDomain = (inWidth < 32 and nbElems == (1ULL << inWidth));
        if (inWidth < 32 and nbElems > (1ULL << inWidth)) {
            std::cerr << "*** Error: Array " << name << " has " << nbElems << " elements but only 2^" << inWidth << " possible indexes" << std::endl;
            exit(EXIT_FAILURE);
        }
        // Values must fit in outWidth bits
        for (uint64_t i = 0; i < nbElems; i += 1) {
            uint64_t v = getContent(i);
            if (outWidth < 64 and (v >> outWidth) != 0) {
                std::cerr << "*** Error: Array " << name << ": value 0x" << std::hex << v << std::dec << " at index " << i << " does not fit on " << outWidth << " bits" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        // Permutation: full domain, same widths, all values distinct
        bool perm = false;
        if (fullDomain and inWidth == outWidth) {
            std::vector<bool> seen(nbElems, false);
            perm = true;
            for (uint64_t i = 0; i < nbElems; i += 1) {
                uint64_t v = getContent(i);
                if (seen[v]) {
                    perm = false;
                    break;
                }
                seen[v] = true;
            }
        }
        if (isPermutation and not perm) {
            std::cerr << "*** Error: Array " << name << " is declared as a permutation but its content is not a permutation of [0, 2^" << inWidth << "[" << std::endl;
            exit(EXIT_FAILURE);
        }
        kind = perm ? ARRAY_KNOWN_PERMUTATION : ARRAY_KNOWN_FUNCTION;
    }
    else {
        if (isPermutation) {
            if (inWidth != outWidth) {
                std::cerr << "*** Error: Array " << name << " is declared as a permutation but its input width (" << inWidth << ") differs from its output width (" << outWidth << ")" << std::endl;
                exit(EXIT_FAILURE);
            }
            kind = ARRAY_DECLARED_PERMUTATION;
        }
        else {
            kind = ARRAY_OPAQUE_FUNCTION;
        }
    }
    maskPreserving = (kind == ARRAY_KNOWN_PERMUTATION or kind == ARRAY_DECLARED_PERMUTATION);
}


std::string ArrayExp::kindStr() const {
    switch (kind) {
        case ARRAY_FUNCTION:
            return "function";
        case ARRAY_KNOWN_PERMUTATION:
            return "known permutation";
        case ARRAY_KNOWN_FUNCTION:
            return "known function";
        case ARRAY_DECLARED_PERMUTATION:
            return "declared permutation";
        case ARRAY_OPAQUE_FUNCTION:
            return "opaque function";
    }
    return "?";
}


ArrayExp::~ArrayExp() {
}


Node & ArrayExp::operator[](Node & child) {
    if (child.width != inWidth) {
        std::cerr << "*** Error: Expression " << child << " has a width of " << child.width << " which cannot be used to index array " << name << " with a width of " << inWidth << std::endl;
        exit(EXIT_FAILURE);
    }
    if (propagateCstOnBuild() and child.nature == CONST and content != NULL) {
        assert(child.nlimbs == 1);
        return Const(getContent(child.cst[0]), outWidth);
    }
    if (func != NULL) {
        return func(child);
    }
    Node & nameNode = Str(name);
    return Node::OpNode(ARRAY, {&nameNode, &child});
}


ArrayExp & ArrayExp::getByName(const std::string & name) {
    auto it = allArrays.find(name);
    if (it == allArrays.end()) {
        std::cerr << "*** Error: No array named " << name << std::endl;
        exit(EXIT_FAILURE);
    }
    return *it->second;
}


uint64_t ArrayExp::getContent(uint64_t idx) {
    // FIXME: check that elemSize has a value that is compatible with outWidth

    if (content == NULL) {
        std::cerr << "*** Error: Array " << name << " has no concrete content" << std::endl;
        exit(EXIT_FAILURE);
    }
    // size is in bytes
    if (elemSize <= 0 or idx >= (uint64_t) (size / elemSize)) {
        std::cerr << "*** Error: Index " << idx << " out of bounds for array " << name << " (" << (elemSize > 0 ? size / elemSize : 0) << " elements)" << std::endl;
        exit(EXIT_FAILURE);
    }

    uint64_t val;
    if (elemSize == 1) {
        val = ((uint8_t *) content)[idx];
    }
    else if (elemSize == 2) {
        val = ((uint16_t *) content)[idx];
    }
    else if (elemSize == 4) {
        val = ((uint32_t *) content)[idx];
    }
    else if (elemSize == 8) {
        val = ((uint64_t *) content)[idx];
    }
    else {
        assert(false);
    }
    return val;
}


void ArrayExp::cleanup() {
    for (const auto & a : registeredArrays) {
        delete a;
    }
    registeredArrays.clear();
    registeredArraysByName.clear();
    registeredArraysByAddr.clear();
    allArrays.clear();
}

} // namespace vmsi
