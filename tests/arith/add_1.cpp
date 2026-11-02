#include "verif_msi_pp.hpp"


int main() {
    // 1. Carry propagation when limb[1] has 0xFFFFFFFFFFFFFFFF + 0xFFFFFFFFFFFFFFFF + carry(1)
    Node & a0 = constant(0xFFFFFFFFFFFFFFFFULL, 64);
    Node & a1 = constant(0xFFFFFFFFFFFFFFFFULL, 64);
    Node & a2 = constant(0x0000000000000001ULL, 64);
    Node & a = Concat(a2, a1, a0);

    Node & b0 = constant(0x0000000000000001ULL, 64);
    Node & b1 = constant(0xFFFFFFFFFFFFFFFFULL, 64);
    Node & b2 = constant(0x0000000000000001ULL, 64);
    Node & b = Concat(b2, b1, b0);

    Node & sum = a + b;
    Node & ref = Concat(
        constant(0x0000000000000003ULL, 64),
        constant(0xFFFFFFFFFFFFFFFFULL, 64),
        constant(0x0000000000000000ULL, 64)
    );
    checkResults(sum, ref);

    // 2. Non-multiple of 64 width (100 bits = 36 + 64) with overflow truncation on top limb
    Node & c0 = Concat(constant(0xFFFFFFFFFULL, 36), constant(0xFFFFFFFFFFFFFFFFULL, 64));
    Node & c1 = constant(2, 100);
    Node & sum2 = c0 + c1;
    Node & ref2 = constant(1, 100);
    checkResults(sum2, ref2);

    return 0;
}

