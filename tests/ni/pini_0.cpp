#include "verif_msi_pp.hpp"


int main() {
    Node & k = symbol("k", 'S', 8);
    Node & m = symbol("m", 'M', 8);
    std::vector<Node *> u = getRealShares(k, 3);

    Node & u0 = *u[0];
    Node & u1 = *u[1];

    bool usedBitExp = false;
    std::set<int> outIdx {0};

    // u0 uses share index 0 (in outIdx), so 0 additional input shares are needed -> PINI with maxShareOcc = 0
    bool res1 = checkPINIVal(u0 ^ m, 0, outIdx, &usedBitExp);
    // u0 ^ u1 uses share index 1 (not in outIdx), so fails for maxShareOcc = 0
    bool res2 = checkPINIVal(u0 ^ u1, 0, outIdx, &usedBitExp);

    if (res1 && !res2) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}

