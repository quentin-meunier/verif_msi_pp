
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k = symbol("k", 'S', 8);
    Node & t = constant(0, 8);
    Node & p = constant(0, 8);

    Node & n0 = (k ^ t);
    Node & n = n0 + p;

    checkResults(n, k);

    delete &k;
    delete &t;

    return 0;
}


