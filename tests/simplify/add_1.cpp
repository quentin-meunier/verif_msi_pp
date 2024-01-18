
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & c0 = constant(0, 8);

    Node & n0 = c0 + k0;

    checkResults(n0, k0);

    n0.dump("graph.dot");

    delete &k0;
    delete &c0;
    delete &n0;

    return 0;
}


