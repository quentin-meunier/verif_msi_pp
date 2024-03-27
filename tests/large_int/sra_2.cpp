
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x11111111, 32);
    Node & c1 = constant(0x22222222, 32);
    Node & c2 = constant(0x33333333, 32);
    Node & c3 = constant(0x84444444, 32);
    Node & n = Concat(c3, c2, c1, c0);
    Node & r = n >> 32;
    Node & ref = Concat(constant(0xFFFFFFFF84444444, 64), constant(0x3333333322222222, 64));

    checkResults(r, ref);

    return 0;
}


