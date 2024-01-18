
#include "node.hpp"
#include "utils.hpp"


int main() {

    // same as k9, but order of xor is different:
    // matters because the xor is binary
    // and k ^ k ^ 1 actually is:
    //     ^
    //    / \
    //   ^   1
    //  / \
    // k   k

    // &          &
    //  \          \
    //   \     ->   \
    //    \          \
    //     ^          \
    //    /|\          \
    //   k k 1          1


    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    Node & c0 = constant(255, 8);

    Node & n0 = k0 ^ k0 ^ c0;
    Node & n = n0 & k1;

    Node & res = k1;


    checkResults(n, res);

    return 0;
}


