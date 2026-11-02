
#include "verif_msi_pp.hpp"


int main() {

    Node & c0 = constant(0xFFFFFFFF, 32);
    Node & c1 = constant(0xF, 32);

    Node & n = c0 * c1;
    Node & res = constant(0xFFFFFFF1, 32);

    checkResults(n, res);

    Node & c2 = constant(0xFFFFFFFFFFFFFFFFULL, 64);
    Node & c3 = constant(0xFULL, 64);
    Node & n2 = c2 * c3;
    Node & res2 = constant(0xFFFFFFFFFFFFFFF1ULL, 64);

    checkResults(n2, res2);

    return 0;
}


