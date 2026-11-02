#include "verif_msi_pp.hpp"


int main() {
    Node & c0 = constant(0x1111111111111111ULL, 64);
    Node & c1 = constant(0x2222222222222222ULL, 64);
    Node & n = Concat(c1, c0);

    // 1. Logical shift right by exact multiple of 64 (LShR by 64)
    Node & r64 = LShR(n, 64);
    Node & ref64 = Concat(constant(0, 64), c1);
    checkResults(r64, ref64);

    // 2. Logical shift right on non-multiple of 64 width (100 bits, LShR by 64)
    Node & n100 = Concat(constant(0xABCDE1234ULL, 36), constant(0x1111111111111111ULL, 64));
    Node & r100 = LShR(n100, 64);
    Node & ref100 = Concat(constant(0, 36), constant(0x0000000ABCDE1234ULL, 64));
    checkResults(r100, ref100);

    return 0;
}

