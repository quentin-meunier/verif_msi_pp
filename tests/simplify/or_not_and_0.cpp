#include "verif_msi_pp.hpp"


int main() {
    // x | ~(x & y) -> 1, with x a '&' node
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);
    Node & y = symbol("y", 'P', 8);

    Node & x = c & d;
    Node & n = x | ~(x & y);
    Node & res = constant(0xFF, 8);

    checkResults(n, res);

    return 0;
}
