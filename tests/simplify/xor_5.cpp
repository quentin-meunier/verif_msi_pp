
#include "node.hpp"
#include "utils.hpp"


int main() {

    //    root       root
    //     ^          ~
    //    /|\   ->    |
    //   1            ^
    //                |\


    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & c0 = constant(255, 8);

    Node & n = k0 ^ k1 ^ c0;

    Node & res0 = k0 ^ k1;
    Node & res = ~res0;


    checkResults(n, res);

    return 0;
}


