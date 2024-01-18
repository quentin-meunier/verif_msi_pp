
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 32);
    Node & p = symbol("p", 'P', 32);

    Node & n0 = k ^ p;

    Node & e0 = Extract(7, 0, n0);
    Node & e1 = Extract(15, 8, n0);
    Node & e2 = Extract(23, 16, n0);
    Node & e3 = Extract(31, 24, n0);

    Node & n1 = Concat(e3, e2, e1, e0);

    checkResults(n1, n0);

    return 0;
}


