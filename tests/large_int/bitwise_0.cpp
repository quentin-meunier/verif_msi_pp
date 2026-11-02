#include "verif_msi_pp.hpp"


int main() {
    // 1. Bitwise NOT, AND, OR, XOR on 100-bit constants (width % 64 != 0)
    Node & a = Concat(constant(0x0F0F0F0F0ULL, 36), constant(0xAAAAAAAAAAAAAAAAULL, 64));
    Node & b = Concat(constant(0xF0F0F0F0FULL, 36), constant(0x5555555555555555ULL, 64));
    Node & allOnes = Concat(constant(0xFFFFFFFFFULL, 36), constant(0xFFFFFFFFFFFFFFFFULL, 64));
    Node & allZeros = constant(0, 100);

    checkResults(~a, b);
    checkResults(a ^ b, allOnes);
    checkResults(a | b, allOnes);
    checkResults(a & b, allZeros);

    // 2. Simplification with isAllOne and isZero on > 64-bit symbols (65 bits)
    Node & s = symbol("s", 'S', 65);
    Node & ones65 = ~constant(0, 65);
    Node & zero65 = constant(0, 65);

    checkResults(s & ones65, s);
    checkResults(s | zero65, s);
    checkResults(s ^ zero65, s);
    checkResults(s ^ ones65, ~s);

    return 0;
}

