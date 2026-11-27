#include "verif_msi_pp.hpp"


int main() {
    // (a | b) | ~(a | b) -> 1 (the inner '|' is merged into the outer one)
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);

    Node & n = (a | b) | ~(a | b);
    Node & res = constant(0xFF, 8);

    checkResults(n, res);

    return 0;
}
