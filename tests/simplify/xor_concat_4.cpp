
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    /*
    +------------------------------------------+
    |               cst                        |
    +------------------------------------------+

    +------------------------------------------+
    |   cst   | a |   cst   |  b |   cst       |
    +------------------------------------------+

    +------------------------------------------+
    |  c  |   cst    | d |  cst  |   e   | cst |
    +------------------------------------------+

    */
    Node & a = symbol("a", 'P', 2);
    Node & b = symbol("b", 'P', 2);
    Node & c = symbol("c", 'P', 2);
    Node & d = symbol("d", 'P', 2);
    Node & e = symbol("e", 'P', 2);
    Node & f = symbol("f", 'P', 2);
    Node & g = symbol("g", 'P', 2);

    Node & cst64 = constant(0xffffffffffffffff, 64);
    Node & n0 = simplify(Concat(cst64, cst64));

    Node & cst4 = constant(0xf, 4);
    Node & cst8 = constant(0xff, 8);
    Node & cst48 = constant(0xffffffffffff, 48);
    Node & cst112 = Concat(cst64, cst48);
    Node & n1 = Concat(cst4, a, cst8, b, cst112);

    Node & cst7 = constant(0x7f, 7);
    Node & cst5 = constant(0x1f, 5);
    Node & cst46 = constant(0x3fffffffffff, 46);
    Node & cst110 = Concat(cst64, cst46);
    Node & n2 = Concat(c, cst7, d, cst5, e, cst110);

    Node & n = n0 ^ n1 ^ n2;

    Node & cst2 = constant(0x3, 2);
    Node & cst3 = constant(0x7, 3);
    Node & res = Concat(c, cst2, a, cst3, d, cst3, b, e, cst110);

    checkResults(n, res);

    return 0;
}


