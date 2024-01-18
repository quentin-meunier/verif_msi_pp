
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & c0 = constant(4, 8);
    Node & c1 = constant(5, 8);

    Node n0 = k0 + c0;
    n0 = c0 + c1;
    n0 = k0 + c0;
    n0 = c0 + c1;
    n0 = k0 + c0;
    n0 = c0 + c1;

    //checkResults(n0, k0);

    n0.dump("graph.dot");

    delete &k0;
    delete &c0;
    delete &c1;
    //delete &n0;

    return 0;
}


