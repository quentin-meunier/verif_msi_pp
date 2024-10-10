
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);

    Node & n = ~a | ~b | ~c | ~(a & ~b);
    Node & res = constant(0xFF, 8);

    checkResults(n, res);

    return 0;
}


