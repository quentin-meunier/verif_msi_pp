
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x12345678, 32);
    Node & c1 = constant(8, 8);
    Node & c2 = constant(15, 8);
    Node & n = Extract(c2, c1, c0);

    Node & res = constant(0x56, 8);

    checkResults(n, res);

    return 0;
}


