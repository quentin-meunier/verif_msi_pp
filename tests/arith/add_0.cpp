
#include "verif_msi_pp.hpp"


int main() {

    Node & c0 = constant(0xFFFFFFFFFFFFFFFF, 64);
    Node & c1 = constant(0x7FFFFFFFFFFFFFFF, 64);
    Node & n0 = Concat(c1, c0);

    Node & n1 = constant(1, 128);
    Node & n = n0 + n1;

    // result is 0x80000000000000000000000000000000
    Node & res0 = constant(0x8000000000000000, 64);
    Node & res1 = constant(0x0000000000000000, 64);
    Node & res = Concat(res0, res1);

    checkResults(n, res);

    return 0;
}


