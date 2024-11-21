
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a0 = symbol("a0", 'P', 8);
    Node & a1 = symbol("a1", 'P', 8);
    Node & a2 = symbol("a2", 'P', 8);
    Node & b = symbol("b", 'P', 8);

    Node & n = a0 | ~a1 | a2 | (~(a0 | ~a1 | a2) & b);
    Node & res = a0 | ~a1 | a2 | b;

    checkResults(n, res);

    return 0;
}


