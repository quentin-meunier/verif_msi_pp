#include "verif_msi_pp.hpp"


int main() {
    Node & c0 = constant(0x1111111111111111ULL, 64);
    Node & c1 = constant(0x2222222222222222ULL, 64);
    Node & n = Concat(c1, c0);

    // 1. Shift left by exact multiple of 64 (<< 64)
    Node & r64 = n << 64;
    Node & ref64 = Concat(c0, constant(0, 64));
    checkResults(r64, ref64);

    // 2. Shift left across limb boundary on non-multiple of 64 width (100 bits, << 72)
    Node & n100 = Concat(constant(0x123456789ULL, 36), constant(0xABCDEF0123456789ULL, 64));
    Node & r72 = n100 << 72;
    Node & ref72 = Concat(constant(0x345678900ULL, 36), constant(0, 64));
    checkResults(r72, ref72);

    return 0;
}

