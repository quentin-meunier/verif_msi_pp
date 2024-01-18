
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);

    Node & k = symbol("k", 'S', 8);

    Node & n0 = m0 ^ k;
    n0.printMaskOcc();

    Node & n1 = m1 + n0;
    n1.printMaskOcc();

    Node & e = n1 + m0;

    e.printMaskOcc();
    e.dump("graph.dot");

    delete &m0;
    delete &m1;
    delete &k;
    delete &n0;
    delete &n1;
    delete &e;

    return 0;
}


