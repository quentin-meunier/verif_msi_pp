#include "verif_msi_pp.hpp"


int main() {
    // 1. Bit decomposition of a 100-bit constant (64 + 36 bits, non-multiple of 64)
    Node & cHigh = constant(0x5A5A5A5A5ULL, 36);
    Node & cLow = constant(0x123456789ABCDEF0ULL, 64);
    Node & c100 = Concat(cHigh, cLow);
    Node & bd100 = getBitDecomposition(c100);
    checkResults(bd100, c100);

    // 2. Bit decomposition of a 65-bit constant (boundary 64 + 1 bits)
    Node & c65 = Concat(constant(1, 1), constant(0x8000000000000001ULL, 64));
    Node & bd65 = getBitDecomposition(c65);
    checkResults(bd65, c65);

    // 3. Bit decomposition of a 130-bit constant (3 limbs: 64 + 64 + 2 bits)
    Node & c130 = Concat(
        constant(0x3ULL, 2),
        constant(0xFEDCBA9876543210ULL, 64),
        constant(0x0123456789ABCDEFULL, 64)
    );
    Node & bd130 = getBitDecomposition(c130);
    checkResults(bd130, c130);

    return 0;
}

