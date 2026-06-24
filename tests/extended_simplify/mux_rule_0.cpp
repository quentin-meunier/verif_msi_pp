
#include "verif_msi_pp.hpp"


int main() {
    Node & x = symbol("x", 'P', 1);
    Node & y = symbol("y", 'P', 1);
    Node & p = symbol("p", 'P', 1);

    Node & e = (~p & x) ^ (p & y);
    

    Node & res = x ^ (p & (x ^ y));

    checkResults(e, res);

    return 0;
}

