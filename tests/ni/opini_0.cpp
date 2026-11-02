#include "verif_msi_pp.hpp"


int main() {
    Node & a = symbol("a", 'S', 1);
    Node & b = symbol("b", 'S', 1);
    Node & m = symbol("m", 'M', 1);

    std::vector<Node *> ua = getRealShares(a, 2);
    std::vector<Node *> ub = getRealShares(b, 2);

    Node & a0 = *ua[0];
    Node & a1 = *ua[1];
    Node & b0 = *ub[0];

    bool usedBitExp = false;
    std::set<int> outIdx {0};
    std::vector<std::vector<Node *>> outLeaks {{&(a0 ^ m), &(a1 ^ m)}};

    bool res1 = checkOPINIVal(a0 & b0, 0, outIdx, outLeaks, &usedBitExp);
    bool res2 = checkOPINIVal(a0 ^ a1, 0, outIdx, outLeaks, &usedBitExp);

    if (res1 && !res2) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}

