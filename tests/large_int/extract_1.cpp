
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & c0 = constant(0x11111111, 32);
    Node & c1 = constant(0x22222222, 32);
    Node & c2 = constant(0x33333333, 32);
    Node & c3 = constant(0x44444444, 32);
    Node & n = Concat(c3, c2, c1, c0);
    Node & u = Extract(7, 0, n);
    Node & v = Extract(39, 32, n);
    Node & w = Extract(71, 64, n);
    Node & x = Extract(103, 96, n);

    if (equivalence(constant(0x11, 8), u) && equivalence(constant(0x22, 8), v) && equivalence(constant(0x33, 8), w) && equivalence(constant(0x44, 8), x)) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


