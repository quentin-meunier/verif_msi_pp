
#include "verif_msi_pp.hpp"


int main() {
    Node & m = symbol("m", 'M', 8);
    Node & k = symbol("k", 'S', 8);

    Node & n0 = k ^ m;
    std::cout << "# Masks Occurrences of node: " << n0 << std::endl;
    n0.printMaskOcc();

    checkMaskOcc(
        n0,
        {NULL, NULL},
        {{&m, &n0}},
        {{&m, {{&n0, {{&n0, {1, 0}}}}}}},
        {}
    );

    Node & n1 = ~n0;
    std::cout << "# Masks Occurrences of node: " << n1 << std::endl;
    n1.printMaskOcc();

    checkMaskOcc(
        n1,
        {NULL, NULL},
        {{&m, &n0}},
        {{&m, {{&n0, {{&n0, {1, 0}}, {&n1, {1, 1}}}}}}},
        {}
    );

    Node & e = n1 + n1;
    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();

    checkMaskOcc(
        e,
        {NULL, NULL},
        {},
        {{&m, {{&n0, {{&n0, {2, 0}}, {&n1, {2, 1}}}}}}},
        {}
    );

    e.dump("graph.dot");

    return 0;
}


