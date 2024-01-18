
#include "node.hpp"
#include "utils.hpp"


int main() {

    // k0 ^ m0 ^ m1 ^ m2 ^ m0 ^ m1 ^ m0
    Node & k0 = symbol("k0", 'S', 8);
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);
    Node & m2 = symbol("m2", 'M', 8);

    Node & n = k0 ^ m0 ^ m1 ^ m2 ^ m0 ^ m1 ^ m0;

    Node & res = k0 ^ m0 ^ m2;

    checkResults(n, res);

    return 0;
}


