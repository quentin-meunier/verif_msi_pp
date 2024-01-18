
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(2, 8);
    Node & c1 = constant(2, 8);
    Node & k0 = symbol("k0", 'S', 8);

    Node & n0 = c0 - c1;
    Node & n = k0 | n0;
    // result is k0
    Node & res = k0;


    checkResults(n, res);

    return 0;
}


