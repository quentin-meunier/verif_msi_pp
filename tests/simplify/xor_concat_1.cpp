
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

    Node & n = Concat(a, b) ^ Concat(c, d) ^ Concat(e, f) ^ Concat(g, h);

    Node & res = Concat(a ^ c ^ e ^ g, b ^ d ^ f ^ h);


    checkResults(n, res);

    return 0;
}


