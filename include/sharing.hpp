/*
Copyright (C) 2026, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#ifndef _sharing_hpp_
#define _sharing_hpp_

#include <cstdint>
#include <unordered_map>
#include <vector>


namespace vmsi {

class Node;

// Associations between secrets, shares and bit symbols.
// They concern only a few symbol nodes and are rarely accessed during the
// verification, so they are kept here rather than in each Node.

struct ShareInfo {
    Node * secret;        // Secret from which the share is derived
    int32_t shareNum;     // Index of the share
    Node * pseudoShareEq; // Equivalent expression using the secret and masks (pseudo-shares)
};

struct BitInfo {
    Node * orig; // Symbol from which the bit symbol is derived
    int32_t bit; // Bit index in orig
};

class Sharing {

    public:
    // share -> (secret, share index, pseudo-share equivalent)
    inline static std::unordered_map<const Node *, ShareInfo> shares;
    // secret -> shares (real shares only)
    inline static std::unordered_map<const Node *, std::vector<Node *>> secretShares;
    // bit symbol -> (original symbol, bit index)
    inline static std::unordered_map<const Node *, BitInfo> bits;

    static void registerShare(const Node & share, Node & secret, int32_t shareNum, Node & pseudoShareEq);
    static void registerSecretShares(const Node & secret, const std::vector<Node *> & shares);
    static void registerBit(const Node & bitSymb, Node & orig, int32_t bit);

    static bool isShare(const Node & n);
    static const ShareInfo & getShareInfo(const Node & share);
    static Node & getSecret(const Node & share);
    static int32_t getShareNum(const Node & share);

    // For a bit of a secret (e.g. k#0), returns the shares of the original secret (k)
    static const std::vector<Node *> & getSecretShares(const Node & secret);
    // For a bit of a secret (e.g. k#0), returns the number of shares of the original secret (k)
    static int32_t getNbShares(const Node & secret);

    static bool isBit(const Node & n);
    static const BitInfo & getBitInfo(const Node & bitSymb);

    static void cleanup();
};

} // namespace vmsi

#endif
