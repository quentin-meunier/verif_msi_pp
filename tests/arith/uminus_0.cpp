#include "verif_msi_pp.hpp"


int main() {
    // 1. Unary minus on 0 (128 bits) -> 0
    Node & z = constant(0, 128);
    checkResults(-z, z);

    // 2. Unary minus when low limb is 0 and high limb is non-zero
    Node & a = Concat(constant(0x0000000000000001ULL, 64), constant(0x0000000000000000ULL, 64));
    Node & refA = Concat(constant(0xFFFFFFFFFFFFFFFFULL, 64), constant(0x0000000000000000ULL, 64));
    checkResults(-a, refA);

    // 3. Unary minus on 100-bit constant (non-multiple of 64)
    Node & b = constant(1, 100);
    Node & refB = Concat(constant(0xFFFFFFFFFULL, 36), constant(0xFFFFFFFFFFFFFFFFULL, 64));
    checkResults(-b, refB);

    return 0;
}

