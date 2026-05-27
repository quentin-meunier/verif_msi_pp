
#include "verif_msi_pp.hpp"


int main() {

    Node & p = symbol("p", 'P', 8);
    Node & pb = getBitDecomposition(p);

    Node & n = pb >> 1;

    Node & e7 = simplify(Extract(7, 7, pb));
    Node & e6 = simplify(Extract(6, 6, pb));
    Node & e5 = simplify(Extract(5, 5, pb));
    Node & e4 = simplify(Extract(4, 4, pb));
    Node & e3 = simplify(Extract(3, 3, pb));
    Node & e2 = simplify(Extract(2, 2, pb));
    Node & e1 = simplify(Extract(1, 1, pb));
    Node & e0 = simplify(Extract(0, 0, pb));
    Node & res = Concat(e7, e7, e6, e5, e4, e3, e2, e1);

    checkResults(n, res);

    return 0;
}

