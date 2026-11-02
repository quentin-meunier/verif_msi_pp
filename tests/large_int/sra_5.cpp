#include "verif_msi_pp.hpp"


int main() {
    // 1. Arithmetic shift right by exact multiple of 64 (>> 64) on 92-bit negative constant
    Node & c0 = constant(0x11111111, 32);
    Node & c1 = constant(0x22222222, 32);
    Node & c2 = constant(0x08444444, 28);
    Node & n92 = Concat(c2, c1, c0);
    Node & r64 = n92 >> 64;
    Node & ref64 = Concat(constant(0x0FFFFFFFULL, 28), constant(0xFFFFFFFFF8444444ULL, 64));
    checkResults(r64, ref64);

    // 2. Arithmetic shift right by 64 on 128-bit negative constant
    Node & n128 = Concat(constant(0x8000000000000001ULL, 64), constant(0x123456789ABCDEF0ULL, 64));
    Node & r128 = n128 >> 64;
    Node & ref128 = Concat(constant(0xFFFFFFFFFFFFFFFFULL, 64), constant(0x8000000000000001ULL, 64));
    checkResults(r128, ref128);

    return 0;
}

