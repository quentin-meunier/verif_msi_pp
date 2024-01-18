
#include "node.hpp"
#include "utils.hpp"


int main() {

    // |          |
    //  \          \
    //   \     ->   \
    //    \          \
    //     ^          \
    //    / \          \
    //   k   k          0

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & n0 = k0 ^ k0;
    Node & n = n0 | k1;

    Node & res = k1;


    checkResults(n, res);

    return 0;
}


