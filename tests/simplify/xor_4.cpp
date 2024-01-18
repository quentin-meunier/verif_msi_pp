
#include "node.hpp"
#include "utils.hpp"


int main() {

    // &          &
    //  \          \
    //   \     ->   ~
    //    \          \
    //     ^          ^
    //    /|\         |\
    //   1          


    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & c0 = constant(255, 8);

    Node & n0 = k0 ^ k1 ^ c0;
    Node & n = n0 & k0;
 
    Node & res0 = k0 ^ k1;
    Node & res1 = ~res0;
    Node & res = res1 & k0;

    checkResults(n, res);

    return 0;
}


