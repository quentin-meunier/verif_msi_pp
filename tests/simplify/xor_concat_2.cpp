
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & a = symbol("a", 'P', 3);
    Node & b = symbol("b", 'P', 7);
    Node & c = symbol("c", 'P', 3);
    Node & d = symbol("d", 'P', 7);
    Node & e = symbol("e", 'P', 3);
    Node & f = symbol("f", 'P', 7);
    Node & g = symbol("g", 'P', 3);
    Node & h = symbol("h", 'P', 7);

    Node & u = symbol("u", 'P', 10);
    Node & v = symbol("v", 'P', 10);
    Node & w = constant(0x93, 10);

    Node & n = u ^ Concat(a, b) ^ Concat(c, d) ^ v ^ Concat(e, f) ^ Concat(g, h) ^ w;

    Node & res = Concat(a ^ c ^ e ^ g, b ^ d ^ f ^ h) ^ u ^ v ^ w;


    checkResults(n, res);

    return 0;
}


