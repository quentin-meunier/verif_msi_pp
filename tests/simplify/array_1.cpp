
#include "verif_msi_pp.hpp"


int main() {
    Node & p0 = symbol("p0", 'P', 3);
    Node & p1 = symbol("p1", 'P', 3);
    Node & p2 = symbol("p2", 'P', 3);
    ArrayExp a = ArrayExp("a", 3, 3);

    Node & n = a[(p0 ^ p1) << 1 ^ (p1 << 1)];

    Node & res = a[p0 << 1];

    checkResults(n, res, true);

    return 0;
}


