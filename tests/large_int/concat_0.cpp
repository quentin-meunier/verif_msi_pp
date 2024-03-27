
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x11111111, 32);
    Node & c1 = constant(0x22222222, 32);
    Node & n = Concat(c1, c0);

    // result is 0x12345678
    Node & res = constant(0x2222222211111111, 64);

    checkResults(n, res);

    return 0;
}


