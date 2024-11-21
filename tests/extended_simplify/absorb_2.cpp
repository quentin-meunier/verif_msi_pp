
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a0 = symbol("a0", 'P', 8);
    Node & a1 = symbol("a1", 'P', 8);
    Node & a2 = symbol("a2", 'P', 8);
    Node & a3 = symbol("a3", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);

    Node & n = a0 | ~a1 | a2 | a3 | (b & c & ~(a0 | ~a1 | a2));
    Node & res = a0 | ~a1 | a2 | a3 | b & c;

    checkResults(n, res);

    return 0;
}


