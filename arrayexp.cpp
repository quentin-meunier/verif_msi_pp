/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#include <cassert>

#include "arrayexp.hpp"
#include "node.hpp"


ArrayExp::ArrayExp(const std::string & name, int32_t inWidth, int32_t outWidth, uint32_t addr, int32_t size, std::function<Node &(Node *, Node &)> func, void * content, int32_t elemSize) {
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

    allArrays[name] = this;
}

    
ArrayExp::ArrayExp(const std::string & name, int32_t inWidth, int32_t outWidth) {
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

    allArrays[name] = this;
}
 

Node & ArrayExp::operator[](Node & child) {
    if (child.width != inWidth) {
        std::cerr << "*** Error: Expression " << child << " has a width of " << child.width << " which cannot be used to index array " << name << " with a width of " << inWidth << std::endl;
        exit(EXIT_FAILURE);
    }
    if (propagateCstOnBuild() and child.nature == CONST and content != NULL) {
        return Const(getContent(child.cst), outWidth);
    }
    if (func != NULL) {
        return func(NULL, child);
    }
    Node & nameNode = Str(name);
    return Node::OpNode(ARRAY, {&nameNode, &child});
}


uint64_t ArrayExp::getContent(uint64_t idx) {
    // FIXME: check that elemSize has a value that it is compatible with outWidth

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


