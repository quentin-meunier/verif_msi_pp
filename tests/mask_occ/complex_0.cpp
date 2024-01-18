
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & p0 = symbol("p0", 'P', 8);
    Node & p1 = symbol("p1", 'P', 8);


    //              +
    //         ____/ \___
    //        | / \   \  \
    //        |/   &  |   p0
    //        ~    / \/
    //         \  /  /\
    //          \ | or \___
    //           \|/ \     \
    //            ^   p1    +
    //           / \       / \
    //         m0   k0    m1  k1

    Node & n0 = m0 ^ k0;

    Node & n1 = ~n0;

    Node & n2 = n0 | p1;

    Node & n3 = m1 ^ k1;

    Node & n4 = n0 & n3;

    Node & n5bis = n1 + n1;

    Node & n5 = n5bis + n4;

    Node & n6 = n2 + p0;

    Node & e = n5 + n6;

    e.printMaskOcc();
    e.dump("graph.dot");

    delete &m0;
    delete &m1;
    delete &k0;
    delete &k1;
    delete &p0;
    delete &p1;
    delete &n0;
    delete &n1;
    delete &n2;
    delete &n3;
    delete &n4;
    delete &n5;
    delete &n5bis;
    delete &n6;
    delete &e;

    return 0;
}



