
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & m = symbol("m", 'M', 8);
    Node & k = symbol("k", 'S', 8);

    Node & n0 = k ^ m;
    n0.printMaskOcc();

    Node & n1 = ~n0;
    n1.printMaskOcc();

    Node & e = n1 + n1;

    e.printMaskOcc();
    e.dump("graph.dot");

    delete &k;
    delete &m;
    delete &n0;
    delete &n1;
    delete &e;

    return 0;
}


