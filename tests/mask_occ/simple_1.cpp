
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & m = symbol("m", 'M', 8);

    Node & n = ~m;

    n.printMaskOcc();

    Node & e = n + n;

    e.printMaskOcc();

    delete &m;
    delete &n;
    delete &e;

    return 0;
}


