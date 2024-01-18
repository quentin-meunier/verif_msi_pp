
#include "node.hpp"
#include "utils.hpp"


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

    e.printMaskOcc();
    e.dump("graph.dot");

    delete &m0;
    delete &k0;
    delete &k1;
    delete &p0;
    delete &n0;
    delete &n1;
    delete &n2;
    delete &n3;
    delete &e;

    return 0;
}



