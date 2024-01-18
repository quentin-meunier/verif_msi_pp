
#include "node.hpp"
#include "utils.hpp"


int main() {

    // &          &
    //  \          \
    //   \     ->   \
    //    \          \
    //     ^          ~
    //    /|\         |
    //   0 k 1        k

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    Node & c0 = constant(0, 8);
    Node & c1 = constant(255, 8);

    Node & n0 = c0 ^ k0 ^ c1;
    Node & n = n0 & k1;

    Node & res0 = ~k0;
    Node & res = res0 & k1;


    checkResults(n, res);

    return 0;
}


