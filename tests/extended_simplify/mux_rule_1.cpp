
#include "verif_msi_pp.hpp"


int main() {
    Node & x = symbol("x", 'P', 1);
    Node & y = symbol("y", 'P', 1);
    Node & p = symbol("p", 'P', 1);

    Node & e = (~p & x) ^ (p & (y ^ x));
    

    Node & res = x ^ (p & y);

    checkResults(e, res);

    return 0;
}

