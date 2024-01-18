
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 2);
    Node & q = symbol("q", 'P', 2);
    Node & r = symbol("r", 'P', 2);

    Node & n = -q + -p + q - r + p + (p - q) + r - p;

    Node & res = -q;

    checkResults(n, res);

    return 0;
}


