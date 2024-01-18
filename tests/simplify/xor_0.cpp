
#include "node.hpp"
#include "utils.hpp"


int main() {

    // m0 ^ m0
    Node & m0 = symbol("m0", 'M', 8);
    Node & n = m0 ^ m0;

    // result is 0
    Node & res = constant(0, 8);

    checkResults(n, res);

    return 0;
}


