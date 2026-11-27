
#include "verif_msi_pp.hpp"


int main() {
    // b | c | d | (b ^ c ^ d) -> b | c | d
    // When the bits of the '|' are 0, b ^ c ^ d is 0: the xor can be removed,
    // without adding a '~' (the parity of the '~' was wrong for an even number of operands)
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);

    Node & n = b | c | d | (b ^ c ^ d);
    Node & res = b | c | d;

    checkResults(n, res);

    return 0;
}

