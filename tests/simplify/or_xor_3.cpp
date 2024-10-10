
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);

    Node & n = ~a | ~(a ^ b);
    Node & res = ~a | b;

    checkResults(n, res);

    return 0;
}


