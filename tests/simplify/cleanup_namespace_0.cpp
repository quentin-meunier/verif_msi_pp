#define VMSI_NO_USING_NAMESPACE
#include "verif_msi_pp.hpp"
#include <cassert>

static uint8_t sbox_data[256] = {0};

static vmsi::Node & dummy_func(vmsi::Node & idx) {
    return idx;
}

int main(void) {
    vmsi::Node & k = vmsi::symbol("k", 'S', 8);
    vmsi::Node & m = vmsi::symbol("m", 'M', 8);
    std::vector<vmsi::Node *> shares = vmsi::getRealShares(k, 2);
    vmsi::registerArray("sbox", 8, 8, 0x1000, 256, dummy_func, sbox_data, 1);
    vmsi::HWElement & g0 = vmsi::inputGate(*shares[0]);
    vmsi::HWElement & g1 = vmsi::inputGate(*shares[1]);
    vmsi::HWElement & gm = vmsi::inputGate(m);
    vmsi::HWElement & gx0 = vmsi::xorGate(g0, gm);
    vmsi::HWElement & gx1 = vmsi::xorGate(g1, gm);
    std::vector<vmsi::HWElement *> outputs = {&gx0, &gx1};
    int32_t nbCheck = 0;
    int32_t leak = vmsi::checkSecurity(1, false, vmsi::NI, outputs, false, &nbCheck, false);
    assert(leak == 0);

    // Test large constants (> 64 bits) caching via Concat, Extract, ZeroExt, SignExt
    vmsi::Node & c0 = vmsi::constant(0x123456789ABCDEF0ULL, 64);
    vmsi::Node & c1 = vmsi::constant(0xFEDCBA9876543210ULL, 64);
    vmsi::Node & big1 = vmsi::Concat(c1, c0);
    vmsi::Node & big2 = vmsi::Concat(c1, c0);
    assert(&big1 == &big2);
    vmsi::Node & bigExt = vmsi::ZeroExt(64, c0);
    vmsi::Node & bigSext = vmsi::SignExt(64, c1);
    vmsi::Node & subBig = vmsi::Extract(95, 16, big1);
    vmsi::Node & bigNeg = vmsi::Const((uint64_t) -1, 128, true);
    vmsi::Node & bigNeg2 = vmsi::Const((uint64_t) -1, 128, true);
    assert(&bigNeg == &bigNeg2);
    (void) bigExt;
    (void) bigSext;
    (void) subBig;
    assert(!vmsi::Node::bigCst2node.empty());

    assert(vmsi::Node::nodeNum > 0);
    assert(vmsi::HWElement::nodeNum > 0);

    vmsi::verifMSICleanup();

    assert(vmsi::Node::nodeNum == 0);
    assert(vmsi::HWElement::nodeNum == 0);
    assert(vmsi::Node::symb2node.empty());
    assert(vmsi::Node::cst2node.empty());
    assert(vmsi::Node::bigCst2node.empty());
    assert(vmsi::Node::opNodes.empty());
    assert(vmsi::ArrayExp::allArrays.empty());
    assert(vmsi::HWElement::allHWElements.empty());

    // Re-create symbols, shares, and arrays with the exact same names after cleanup
    vmsi::Node & k2 = vmsi::symbol("k", 'S', 8);
    vmsi::Node & m2 = vmsi::symbol("m", 'M', 8);
    std::vector<vmsi::Node *> shares2 = vmsi::getRealShares(k2, 2);
    vmsi::registerArray("sbox", 8, 8, 0x1000, 256, dummy_func, sbox_data, 1);
    (void) shares2;

    vmsi::Node & e = (k2 ^ m2) ^ m2;
    vmsi::checkResults(e, k2);

    vmsi::verifMSICleanup();

    return 0;
}

