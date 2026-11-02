#include "verif_msi_pp.hpp"


int main() {
    // 1. ZeroExt from 36 bits to 100 bits
    Node & c36 = constant(0xABCDE1234ULL, 36);
    Node & z100 = ZeroExt(64, c36);
    Node & refZ100 = Concat(constant(0, 64), c36);
    checkResults(z100, refZ100);

    // 2. SignExt of negative 36-bit constant to 100 bits
    Node & neg36 = constant(0x8BCDE1234ULL, 36);
    Node & s100 = SignExt(64, neg36);
    Node & refS100 = Concat(constant(0xFFFFFFFFFFFFFFFFULL, 64), neg36);
    checkResults(s100, refS100);

    // 3. SignExt of positive 72-bit constant to 136 bits
    Node & pos72 = Concat(constant(0x7FULL, 8), constant(0x123456789ABCDEF0ULL, 64));
    Node & s136 = SignExt(64, pos72);
    Node & refS136 = Concat(constant(0, 64), pos72);
    checkResults(s136, refS136);

    return 0;
}

