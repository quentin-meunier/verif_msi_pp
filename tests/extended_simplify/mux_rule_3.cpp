
#include "verif_msi_pp.hpp"


int main() {
    Node & x = symbol("x", 'P', 1);
    Node & y = symbol("y", 'P', 1);
    Node & p = symbol("p", 'P', 1);
    Node & u = symbol("u", 'P', 1);
    Node & v = symbol("v", 'P', 1);
    Node & w = symbol("w", 'P', 1);
    Node & z = symbol("z", 'P', 1);

    Node & e = (~p & x) ^ u ^ v ^ w ^ z ^ (p & y);
    

    Node & res = x ^ (p & (x ^ y)) ^ u ^ v ^ w ^ z;

    checkResults(e, res);

    return 0;
}

