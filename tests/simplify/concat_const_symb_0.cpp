
#include "verif_msi_pp.hpp"

int main() {
    Node & s0 = symbol("a", 'S', 32);
    Node & c8 = constant(0x99999999, 32);
    Node & c9 = constant(0x00000000, 32);
    Node & n0 = Concat(c9, c9, c9, c9, c9, c9, c9, c9, c9, c9);
    Node & r = getBitDecomposition(simplify(Concat(c9, n0, s0)));
    Node & n2 = Concat(c8, c8, c9, c9, c9, c9, c9, c9, c9, c9);
    Node & r2 = getBitDecomposition(simplify(Concat(c9, n2, c9)));

    //std::cout << "r:  " << simplify(r).verbatimPrint() << std::endl;
    //std::cout << "r2: " << simplify(r2).verbatimPrint() << std::endl;
    //std::cout << "r | r2 : " << simplify(r2 | r).verbatimPrint() << std::endl;

    Node & n = r2 | r;
    Node & res = getBitDecomposition(Concat(c9, n2, s0));

    checkResults(n, res);

    return 0;
}

