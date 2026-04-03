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


class ArrayExp {

    public:
    std::string name;
    int32_t inWidth;
    int32_t outWidth;
    uint32_t addr;
    int32_t size;
    std::function<Node &(Node &)> func;
    void * content;
    int32_t elemSize; // size in bytes of an element in the concrete array

    inline static std::map<std::string, ArrayExp *> allArrays;

    ArrayExp(const std::string & name, int32_t inWidth, int32_t outWidth);
    ArrayExp(const std::string & name, int32_t inWidth, int32_t outWidth, uint32_t addr, int32_t size, std::function<Node &(Node &)> func, void * content, int32_t elemSize);
 
    Node & operator[](Node & child);
    uint64_t getContent(uint64_t idx);

};


#endif

