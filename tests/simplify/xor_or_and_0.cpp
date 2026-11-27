#include "verif_msi_pp.hpp"


int main() {
    // (a | b) & (~a | ~b) -> a ^ b
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);

    Node & n = (a | b) & (~a | ~b);
    Node & res = a ^ b;

    checkResults(n, res);

    return 0;
}
