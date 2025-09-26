
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & k0 = symbol("k0", 'S', 8);
    Node & p0 = symbol("p0", 'P', 8);
    Node & p1 = symbol("p1", 'P', 8);


    //                       +
    //                      / \
    //                     +   p1
    //                    / \
    //                    ~ &
    //                    |/ \
    //                    ^   p0
    //                   / \
    //                 m0   k0

    Node & n0 = m0 ^ k0;

    Node & n1 = ~n0;

    Node & n4 = n0 & p0;

    Node & n5 = n1 + n4;

    Node & e = n5 + p1;

    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();
    e.dump("graph.dot");

    return 0;
}



