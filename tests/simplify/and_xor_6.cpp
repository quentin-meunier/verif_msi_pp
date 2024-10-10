
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);

    Node & n = a & (a ^ b ^ c ^ d) & b;
    Node & res = a & (c ^ d) & b;

    checkResults(n, res);

    return 0;
}


