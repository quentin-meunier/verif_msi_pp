
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"

int main() {
    Node & s0 = symbol("a", 'S', 32);
    Node & c8 = constant(0x99999999, 32);
    Node & c9 = constant(0x00000000, 32);
    Node & n = Concat(c8, c8, c9, c9, c9, c9, c9, c9, c9, c9);
    Node & real = simplify(Concat(c9, n, s0));
    Node & n2 = Concat(c8, c8, c9, c9, c9, c9, c9, c9, c9, c9);
    Node & real2 = simplify(Concat(c9, n2, c9));

    std::cout << "OP1: " << simplify(real).verbatimPrint() << std::endl;
    std::cout << "OP2: " << simplify(real2).verbatimPrint() << std::endl;
    std::cout << "AND: " << simplify(real2 & real).verbatimPrint() << std::endl;

    return 0;
}

