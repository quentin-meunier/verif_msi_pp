
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 8);
    Node & q = symbol("q", 'P', 8);

    Node & c0 = constant(0xA0, 8);
    Node & c1 = constant(0x0A, 8);
    Node & c2 = constant(0xAA, 8);

    Node & n = c0 ^ p ^ c1 ^ q ^ c2;

    Node & res = p ^ q;

    checkResults(n, res);

    return 0;
}


