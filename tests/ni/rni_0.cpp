#include "verif_msi_pp.hpp"


int main() {
    Node & k = symbol("k", 'S', 8);
    Node & m = symbol("m", 'M', 8);
    std::vector<Node *> u = getRealShares(k, 3);

    Node & u0 = *u[0];
    Node & u1 = *u[1];
    Node & u2 = *u[2];

    // 1. With 3 shares and diff = 1, combining 2 shares (u0 ^ u1) is 1-RNI if masked, not 2-RNI
    bool usedBitExp = false;
    bool res1 = checkRNIVal(u0 ^ m, 2, &usedBitExp);
    bool res2 = checkRNIVal(u0 ^ u1 ^ u2, 1, &usedBitExp);

    if (res1 && !res2) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}

