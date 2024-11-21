
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"

int main() {
    Node & s0 = symbol("a", 'S', 32);
    Node & c8 = constant(0x99999999, 32);
    Node & c9 = constant(0x00000000, 32);
    Node & n = Concat(c9, c9, c9, c9, c9, c9, c9, c9, c9, c9);
    Node & r = getBitDecomposition(simplify(Concat(c9, n, s0)));
    Node & n2 = Concat(c8, c8, c9, c9, c9, c9, c9, c9, c9, c9);
    Node & r2 = getBitDecomposition(simplify(Concat(c9, n2, c9)));

    std::cout << "OP1: " << simplify(r).verbatimPrint() << std::endl;
    std::cout << "OP2: " << simplify(r2).verbatimPrint() << std::endl;
    std::cout << "OR : " << simplify(r2 | r).verbatimPrint() << std::endl;

    return 0;
}

