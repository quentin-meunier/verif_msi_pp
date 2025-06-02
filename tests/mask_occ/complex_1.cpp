
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & p0 = symbol("p0", 'P', 8);
    Node & p1 = symbol("p1", 'P', 8);


    //                       +
    //                    __/ \___
    //                   / \   \  \
    //                  /   &  &  p0
    //                  ~  / \/ \_______
    //                  | /  /\         \
    //                  \ | or \___      m0
    //                   \|/ \     \
    //                    ^   p1    +
    //                   / \       / \
    //                 m0   k0    m1  k1

    Node & n0 = m0 ^ k0;

    Node & n1 = ~n0;

    Node & n2 = n0 | p1;

    Node & n3 = m1 ^ k1;

    Node & n4 = n0 & n3;

    Node & n5 = n1 + n4;

    Node & n6 = n2 & m0;

    Node & n7 = n6 + p0;

    Node & e = n5 + n7;

    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();
    e.dump("graph.dot");

    return 0;
}



