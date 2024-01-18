
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & p = symbol("p", 'P', 8);
    Node & q = symbol("q", 'P', 8);

    Node & c0 = constant(0x80, 8);
    Node & c1 = constant(0x01, 8);
    Node & c2 = constant(0x08, 8);
 
    Node & n = c0 | p | c1 | q | c2;

    Node & res = p | q | constant(0x89, 8);

    checkResults(n, res);

    return 0;
}


