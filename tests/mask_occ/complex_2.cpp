
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & p0 = symbol("p0", 'P', 8);

    //          &
    //         / \
    //         \   +
    //          \ / \
    //           ^   p0
    //          / \
    //         /   k1
    //         ~
    //         |
    //         ^
    //        / \
    //      m0   k0


    Node & n0 = m0 ^ k0;

    Node & n1 = ~n0;

    Node & n2 = n1 ^ k1;

    Node & n3 = n2 + p0;

    Node & e = n3 & n2;

    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();
    e.dump("graph.dot");

    return 0;
}



