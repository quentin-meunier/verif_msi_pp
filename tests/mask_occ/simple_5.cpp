
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);
    Node & k = symbol("k", 'S', 8);

    Node & n0 = m0 ^ k;
    std::cout << "# Masks Occurrences of node: " << n0 << std::endl;
    n0.printMaskOcc();

    Node & n1 = m1 + n0;
    std::cout << "# Masks Occurrences of node: " << n1 << std::endl;
    n1.printMaskOcc();

    Node & e = n1 + m0;
    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();

    e.dump("graph.dot");

    return 0;
}


