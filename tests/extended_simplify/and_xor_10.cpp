
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);

    Node & n = (a ^ b ^ c) & (a ^ b ^ c);
    Node & res = a ^ b ^ c;

    checkResults(n, res);

    return 0;
}


