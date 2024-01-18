
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 2);
    Node & q = symbol("q", 'P', 2);
    Node & r = symbol("r", 'P', 2);

    Node & n = -p + q - r + p + (p - q) + r - p;

    Node & res = constant(0, 2);

    checkResults(n, res);

    return 0;
}


