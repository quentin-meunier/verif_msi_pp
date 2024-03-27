
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & c0 = constant(0x11111111, 32);
    Node & c1 = constant(0x22222222, 32);
    Node & c2 = constant(0x33333333, 32);
    Node & c3 = constant(0x44444444, 32);
    Node & n = Concat(c3, c2, c1, c0);
    Node & u = Extract(103, 16, n);
    Node & res = Concat(constant(0x44, 8), c2, c1, constant(0x1111, 16));

    checkResults(u, res);

    return 0;
}


