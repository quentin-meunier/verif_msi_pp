
#include "node.hpp"
#include "utils.hpp"


int main() {

    // &          &
    //  \          \
    //   \     ->   \
    //    \          \
    //     ^          \
    //    /|\          \
    //   m m k          k


    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    Node & k2 = symbol("k2", 'S', 8);
 
    Node & n0 = k0 ^ k0 ^ k1;
    Node & n = n0 & k2;

    Node & res = k1 & k2;


    checkResults(n, res);

    return 0;
}


