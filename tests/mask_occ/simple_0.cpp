
#include "node.hpp"
#include "utils.hpp"



int main() {
    Node & m = symbol("m", 'M', 8);

    Node & e = m + m;

    e.printMaskOcc();

    delete &m;
    delete &e;

    return 0;
}



