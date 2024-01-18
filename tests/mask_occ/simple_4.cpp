
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & n0 = ~m0;
    n0.printMaskOcc();

    Node & n1 = n0 ^ k0;
    n1.printMaskOcc();

    Node & n2 = ~n1;
    n2.printMaskOcc();

    Node & n3 = m1 ^ n2;
    n3.printMaskOcc();

    Node & e = k1 ^ n3;
    e.printMaskOcc();

    e.dump("graph.dot");

    delete &m0;
    delete &m1;
    delete &k0;
    delete &k1;
    delete &n0;
    delete &n1;
    delete &n2;
    delete &n3;
    delete &e;

    return 0;
}


