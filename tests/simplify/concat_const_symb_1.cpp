
#include "verif_msi_pp.hpp"

int main() {
    Node & s0 = symbol("a", 'S', 32);
    Node & c8 = constant(0x99999999, 32);
    Node & c9 = constant(0x00000000, 32);
    Node & n1 = Concat(c8, c8, c9, c9, c9, c9, c9, c9, c9, c9);
    Node & r = simplify(Concat(c9, n1, s0));
    Node & n2 = Concat(c8, c8, c9, c9, c9, c9, c9, c9, c9, c9);
    Node & r2 = simplify(Concat(c9, n2, c9));

    //std::cout << "OP1: " << simplify(r).verbatimPrint() << std::endl;
    //std::cout << "OP2: " << simplify(r2).verbatimPrint() << std::endl;
    //std::cout << "AND: " << simplify(r2 & r).verbatimPrint() << std::endl;

    Node & n = r2 & r;
    Node & res = r2;

    checkResults(n, res);

    return 0;
}

