
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 2);
    Node & m1 = symbol("m1", 'M', 2);
    Node & m2 = symbol("m2", 'M', 2);
    Node & p0 = symbol("p0", 'P', 2);
    Node & p1 = symbol("p1", 'P', 2);
    Node & a = symbol("a", 'S', 2);
    Node & b = symbol("b", 'S', 2);
    Node & c = symbol("c", 'S', 2);

    std::vector<Node *> aShares = getRealShares(a, 4);
    std::vector<Node *> bShares = getRealShares(b, 4);
    std::vector<Node *> cShares = getRealShares(c, 4);

    Node & a0 = *aShares[0];
    Node & a1 = *aShares[1];
    Node & a2 = *aShares[2];
    Node & a3 = *aShares[3];

    Node & b0 = *bShares[0];
    Node & b1 = *bShares[1];
    Node & b2 = *bShares[2];
    Node & b3 = *bShares[3];

    Node & c0 = *cShares[0];
    Node & c1 = *cShares[1];
    Node & c2 = *cShares[2];
    Node & c3 = *cShares[3];

    bool res;

    Node & e0 = (a0 ^ a1 ^ b0) & p0 & (c0 ^ c1 ^ b0);
    res = isNIWithExev(e0, 2);

    if (res) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    Node & e1 = (a0 ^ a1 ^ b0) & p0 & (c0 ^ c1 ^ b0) & (a0 ^ a1 ^ a2 ^ m0);
    res = isNIWithExev(e1, 2);

    if (res) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    return 0;
}


