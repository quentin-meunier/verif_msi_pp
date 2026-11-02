#include "verif_msi_pp.hpp"


int main() {
    // 1. Subtraction with borrow across 64-bit limb boundary (0x8000000000000000_0000000000000000 - 1)
    Node & a = Concat(constant(0x8000000000000000ULL, 64), constant(0x0ULL, 64));
    Node & b = constant(1, 128);
    Node & diff = a - b;
    Node & ref = Concat(constant(0x7FFFFFFFFFFFFFFFULL, 64), constant(0xFFFFFFFFFFFFFFFFULL, 64));
    checkResults(diff, ref);

    // 2. Subtraction without borrow when low limb is 0xFFFFFFFFFFFFFFFF - 0
    Node & c = Concat(constant(0x0000000000000005ULL, 64), constant(0xFFFFFFFFFFFFFFFFULL, 64));
    Node & d = Concat(constant(0x0000000000000002ULL, 64), constant(0x0000000000000000ULL, 64));
    Node & diff2 = c - d;
    Node & ref2 = Concat(constant(0x0000000000000003ULL, 64), constant(0xFFFFFFFFFFFFFFFFULL, 64));
    checkResults(diff2, ref2);

    // 3. Subtraction on non-multiple of 64 width (100 bits): 0 - 1 = (2^100 - 1)
    Node & z = constant(0, 100);
    Node & one = constant(1, 100);
    Node & diff3 = z - one;
    Node & ref3 = Concat(constant(0xFFFFFFFFFULL, 36), constant(0xFFFFFFFFFFFFFFFFULL, 64));
    checkResults(diff3, ref3);

    return 0;
}

