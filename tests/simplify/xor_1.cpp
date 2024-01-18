
#include "node.hpp"
#include "utils.hpp"


int main() {

    // k0 ^ k0 ^ m0
    Node & m0 = symbol("m0", 'M', 8);
    Node & k0 = symbol("k0", 'S', 8);

    Node & n0 = k0 ^ m0;
    Node & n = n0 ^ k0;

    checkResults(n, m0);

    return 0;
}


