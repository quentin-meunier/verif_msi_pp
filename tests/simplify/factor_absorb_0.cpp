
#include "verif_msi_pp.hpp"


int main() {
    // (x & y) & (x | y) -> x & y
    // After the absorption of (x | y), the loop of factorize() must not go on
    // with the index of the erased child (it then also removed x)
    // Note: the bug depends on the order of the children (i.e. on the hashes),
    // it does not appear with names a and b for instance
    Node & x = symbol("x", 'P', 8);
    Node & y = symbol("y", 'P', 8);

    Node & n = (x & y) & (x | y);
    Node & res = x & y;

    checkResults(n, res);

    return 0;
}

