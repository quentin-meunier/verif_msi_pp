
#include "verif_msi_pp.hpp"



int main() {
    Node & m = symbol("m", 'M', 8);

    Node & e = m + m;
    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();

    checkMaskOcc(
        e,
        {NULL, NULL},
        {},
        {},
        {{&m, {&e}}}
    );

    return 0;
}



