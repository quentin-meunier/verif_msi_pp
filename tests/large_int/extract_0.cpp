
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & c0 = constant(0x11111111, 32);
    Node & c1 = constant(0x22222222, 32);
    Node & c2 = constant(0x33333333, 32);
    Node & c3 = constant(0x44444444, 32);
    Node & n = Concat(c3, c2, c1, c0);
    Node & u = Extract(31, 0, n);
    Node & v = Extract(63, 32, n);
    Node & w = Extract(95, 64, n);
    Node & x = Extract(127, 96, n);

    if (equivalence(c0, u) && equivalence(c1, v) && equivalence(c2, w) && equivalence(c3, x)) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


