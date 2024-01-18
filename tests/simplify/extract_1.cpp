
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 32);
    Node & k0 = Extract(15, 8, k);
    Node & p = symbol("P", 'P', 8);
    Node & n = k0 ^ p;

    n.dump("graph.dot");

    return 0;
}


