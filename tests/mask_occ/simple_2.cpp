
#include "verif_msi_pp.hpp"


int main() {
    Node & m = symbol("m", 'M', 8);

    Node & n0 = ~m;
    Node & n1 = ~m;

    std::cout << "# Masks Occurrences of node: " << n0 << std::endl;
    n0.printMaskOcc();

    checkMaskOcc(
        n0,
        {&m, &n0},
        {},
        {},
        {{&m, {&n0}}}
    );

    Node & e = n0 + n1;
    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();

    checkMaskOcc(
        e,
        {NULL, NULL},
        {},
        {},
        {{&m, {&n0}}}
    );

    e.dump("graph.dot");

    return 0;
}


