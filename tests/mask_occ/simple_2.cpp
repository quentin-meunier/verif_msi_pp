
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & m = symbol("m", 'M', 8);

    Node & n0 = ~m;
    Node & n1 = ~m;

    n0.printMaskOcc();
    n1.printMaskOcc();

    Node & e = n0 + n1;

    e.printMaskOcc();

    e.dump("graph.dot");

    delete &m;
    delete &n0;
    delete &n1;
    delete &e;

    return 0;
}


