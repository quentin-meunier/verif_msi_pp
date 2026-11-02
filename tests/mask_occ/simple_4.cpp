
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & n0 = ~m0;
    std::cout << "# Masks Occurrences of node: " << n0 << std::endl;
    n0.printMaskOcc();

    checkMaskOcc(
        n0,
        {&m0, &n0},
        {},
        {},
        {{&m0, {&n0}}}
    );

    Node & n1 = n0 ^ k0;
    std::cout << "# Masks Occurrences of node: " << n1 << std::endl;
    n1.printMaskOcc();

    checkMaskOcc(
        n1,
        {NULL, NULL},
        {{&m0, &n1}},
        {{&m0, {{&n1, {{&n1, {1, 0}}}}}}},
        {}
    );

    Node & n2 = ~n1;
    std::cout << "# Masks Occurrences of node: " << n2 << std::endl;
    n2.printMaskOcc();

    checkMaskOcc(
        n2,
        {NULL, NULL},
        {{&m0, &n1}},
        {{&m0, {{&n1, {{&n1, {1, 0}}, {&n2, {1, 1}}}}}}},
        {}
    );

    Node & n3 = m1 ^ n2;
    std::cout << "# Masks Occurrences of node: " << n3 << std::endl;
    n3.printMaskOcc();

    checkMaskOcc(
        n3,
        {NULL, NULL},
        {{&m0, &n1}, {&m1, &n3}},
        {
            {&m0, {{&n1, {{&n1, {1, 0}}, {&n2, {1, 1}}, {&n3, {1, 2}}}}}},
            {&m1, {{&n3, {{&n3, {1, 0}}}}}}
        },
        {}
    );

    Node & e = k1 ^ n3;
    std::cout << "# Masks Occurrences of node: " << e << std::endl;
    e.printMaskOcc();

    checkMaskOcc(
        e,
        {NULL, NULL},
        {{&m0, &n1}, {&m1, &n3}},
        {
            {&m0, {{&n1, {{&n1, {1, 0}}, {&n2, {1, 1}}, {&n3, {1, 2}}, {&e, {1, 3}}}}}},
            {&m1, {{&n3, {{&n3, {1, 0}}, {&e, {1, 1}}}}}}
        },
        {}
    );

    e.dump("graph.dot");

    return 0;
}


