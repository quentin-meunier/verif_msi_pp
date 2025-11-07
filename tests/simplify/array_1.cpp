
#include "verif_msi_pp.hpp"


int main() {
    Node & p0 = symbol("p0", 'P', 3);
    Node & p1 = symbol("p1", 'P', 3);
    Node & p2 = symbol("p2", 'P', 3);
    ArrayExp a = ArrayExp("a", 3, 3);

    Node & f = p0 ^ p1 ^ p2 ^ p0;
    Node & n = a[(p0 ^ p1) << 1 ^ (p1 << 1)];

    Node & res = a[p0 << 1];

    Node & t = p0 << 1;
    std::cout << simplifyUSBV(t) << std::endl;
    //std::cout << getBitDecomposition(t) << std::endl;
    //Node & test2 = getBitDecomposition(res);
    //std::cout << test2 << std::endl;
    //Node & test = simplifyUsingSingleBitVariables(res);
    //checkResults(n, res, true, true);

    return 0;
}


