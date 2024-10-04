
#include "node.hpp"
#include "utils.hpp"


int main() {

    /*
    +------------------------------------+
    | cst |    a    |  cst |  b  |   cst |
    +------------------------------------+

    +------------------------------------+
    | cst |    c    |  cst |  d  |   cst |
    +------------------------------------+

    +------------------------------------+
    | e   |   cst   |  f   | cst |   g   |
    +------------------------------------+

    */
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 2);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 2);
    Node & e = symbol("e", 'P', 4);
    Node & f = symbol("f", 'P', 4);
    Node & g = symbol("g", 'P', 6);

    Node & cst0 = constant(0xf, 4);
    Node & cst1 = constant(0xff, 8);
    Node & cst2 = constant(0x3, 2);
    Node & cst3 = constant(0x3f, 6);

    Node & n = Concat(cst0, a, cst0, b, cst3) ^ Concat(cst0, c, cst0, d, cst3) ^ Concat(e, cst1, f, cst2, g);

    Node & res = Concat(e, ~(a ^ c), f, ~(b ^ d), g);

    checkResults(n, res);

    return 0;
}


