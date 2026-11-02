
#include "verif_msi_pp.hpp"


int main() {
    Node & m = symbol("m", 'M', 8);

    Node & n = ~m;
    std::cout << "# Masks Occurrences of node: " << n << std::endl;
    n.printMaskOcc();

    checkMaskOcc(
        n,
        {&m, &n},
        {},
        {},
        {{&m, {&n}}}
    );

    Node & e = n + n;
    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();

    checkMaskOcc(
        e,
        {NULL, NULL},
        {},
        {},
        {{&m, {&n}}}
    );

    return 0;
}


