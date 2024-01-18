
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & c0 = constant(1, 8);
    Node & c1 = constant(5, 8);
    Node & c2 = constant(6, 8);

    Node & n0 = c0 + c1;

    checkResults(n0, c2);

    n0.dump("graph.dot");

    delete &c0;
    delete &c1;
    delete &c2;

    return 0;
}


