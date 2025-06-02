
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & n0 = ~m0;
    std::cout << "# Masks Occurrences of node: " << n0 << std::endl;
    n0.printMaskOcc();

    Node & n1 = n0 ^ k0;
    std::cout << "# Masks Occurrences of node: " << n1 << std::endl;
    n1.printMaskOcc();

    Node & n2 = ~n1;
    std::cout << "# Masks Occurrences of node: " << n2 << std::endl;
    n2.printMaskOcc();

    Node & n3 = m1 ^ n2;
    std::cout << "# Masks Occurrences of node: " << n3 << std::endl;
    n3.printMaskOcc();

    Node & e = k1 ^ n3;
    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();

    e.dump("graph.dot");

    return 0;
}


