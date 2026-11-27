/*
Copyright (C) 2026, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#include <cassert>
#include <cstdlib>
#include <iostream>

#include "sharing.hpp"
#include "node.hpp"


namespace vmsi {

void Sharing::registerShare(const Node & share, Node & secret, int32_t shareNum, Node & pseudoShareEq) {
    if (shares.contains(&share)) {
        std::cerr << "*** Error: share " << share << " has already been registered" << std::endl;
        exit(EXIT_FAILURE);
    }
    shares.emplace(&share, ShareInfo{&secret, shareNum, &pseudoShareEq});
}


void Sharing::registerSecretShares(const Node & secret, const std::vector<Node *> & sh) {
    if (secretShares.contains(&secret)) {
        std::cerr << "*** Error: shares of secret " << secret << " have already been registered" << std::endl;
        exit(EXIT_FAILURE);
    }
    secretShares.emplace(&secret, sh);
}


void Sharing::registerBit(const Node & bitSymb, Node & orig, int32_t bit) {
    // The same bit symbol can be obtained several times (e.g. if the cache in the Extract node has been deleted)
    auto it = bits.find(&bitSymb);
    if (it != bits.end()) {
        assert(it->second.orig == &orig && it->second.bit == bit);
        return;
    }
    bits.emplace(&bitSymb, BitInfo{&orig, bit});
}


bool Sharing::isShare(const Node & n) {
    return shares.contains(&n);
}


const ShareInfo & Sharing::getShareInfo(const Node & share) {
    auto it = shares.find(&share);
    if (it == shares.end()) {
        std::cerr << "*** Error: " << share << " is not a share" << std::endl;
        exit(EXIT_FAILURE);
    }
    return it->second;
}


Node & Sharing::getSecret(const Node & share) {
    return *getShareInfo(share).secret;
}


int32_t Sharing::getShareNum(const Node & share) {
    return getShareInfo(share).shareNum;
}


const std::vector<Node *> & Sharing::getSecretShares(const Node & secret) {
    const Node * s = &secret;
    while (true) {
        auto it = secretShares.find(s);
        if (it != secretShares.end()) {
            return it->second;
        }
        auto itBit = bits.find(s);
        if (itBit == bits.end()) {
            std::cerr << "*** Error: secret " << secret << " has not been split into real shares" << std::endl;
            exit(EXIT_FAILURE);
        }
        s = itBit->second.orig;
    }
}


int32_t Sharing::getNbShares(const Node & secret) {
    return (int32_t) getSecretShares(secret).size();
}


bool Sharing::isBit(const Node & n) {
    return bits.contains(&n);
}


const BitInfo & Sharing::getBitInfo(const Node & bitSymb) {
    auto it = bits.find(&bitSymb);
    if (it == bits.end()) {
        std::cerr << "*** Error: " << bitSymb << " is not a bit symbol" << std::endl;
        exit(EXIT_FAILURE);
    }
    return it->second;
}


void Sharing::cleanup() {
    shares.clear();
    secretShares.clear();
    bits.clear();
}

} // namespace vmsi
