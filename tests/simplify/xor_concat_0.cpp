
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & a = symbol("a", 'P', 3);
    Node & b = symbol("b", 'P', 7);
    Node & c = symbol("c", 'P', 11);
    Node & d = symbol("d", 'P', 5);
    Node & e = symbol("e", 'P', 3);
    Node & f = symbol("f", 'P', 7);
    Node & g = symbol("g", 'P', 11);
    Node & h = symbol("h", 'P', 5);

    Node & n = Concat(a, b, c, d) ^ Concat(e, f, g, h);

    Node & res = Concat(a ^ e, b ^ f, c ^ g, d ^ h);

    checkResults(n, res);

    return 0;
}


