
#include "verif_msi_pp.hpp"


int main() {
    // (a2 ^ r1) & (r0 ^ r1) & (a0 ^ a2 ^ r0 ^ r1) -> (a2 ^ r1) & (r0 ^ r1) & (a0 ^ r1)
    // When the bits of the '&' are 1: a0 ^ a2 ^ r0 ^ r1 = a0 ^ (a2 ^ r1) ^ (r0 ^ r1) ^ r1 = a0 ^ r1
    // r1 is removed twice, so it must stay (symmetric difference of the removed children)
    Node & a0 = symbol("a0", 'P', 8);
    Node & a2 = symbol("a2", 'P', 8);
    Node & r0 = symbol("r0", 'P', 8);
    Node & r1 = symbol("r1", 'P', 8);

    Node & n   = (a2 ^ r1) & (r0 ^ r1) & (a0 ^ a2 ^ r0 ^ r1);
    Node & res = (a2 ^ r1) & (r0 ^ r1) & (a0 ^ r1);

    checkResults(n, res);

    return 0;
}

