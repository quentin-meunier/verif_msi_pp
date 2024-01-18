
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x78, 8);
    Node & c1 = constant(0x56, 8);
    Node & c2 = constant(0x34, 8);
    Node & c3 = constant(0x12, 8);
    Node & c4 = constant(0x12345678, 32);
    Node & n = Concat(c3, c2, c1, c0);

    // result is 0x12345678
    Node & res = c4;

    checkResults(n, res);

    return 0;
}


