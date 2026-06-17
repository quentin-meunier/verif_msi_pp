
#include "verif_msi_pp.hpp"


int main() {

    Node & p = symbol("p", 'P', 4);
    Node & pb = getBitDecomposition(p);

    Node & n0 = Concat(pb, constant(0, 4));
    Node & n1 = simplify(n0 << 2);

    Node & e1 = simplify(Extract(1, 1, pb));
    Node & e0 = simplify(Extract(0, 0, pb));
    Node & res = Concat(e1, e0, constant(0, 6));

    checkResults(n1, res, true);

    return 0;
}

