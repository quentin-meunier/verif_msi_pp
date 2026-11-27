#include "verif_msi_pp.hpp"


int main() {
    Node & a = symbol("a", 'S', 2);
    Node & b = symbol("b", 'S', 2);
    Node & z = symbol("z", 'M', 2);
    std::vector<Node *> as = getRealShares(a, 3);
    std::vector<Node *> bs = getRealShares(b, 3);

    // With 3 shares and diff = 1, a tuple using 1 share of each secret is RNI.
    // With bit decomposition, the secrets of the bits (a#0, a#1, ...) must have the same number of shares as a
    std::vector<Node *> v0 = {as[1], bs[0], &z};
    // 2 shares of a with diff = 1: not RNI
    std::vector<Node *> v1 = {as[0], as[1], &z};

    bool res0 = rni(v0, 1, true);
    bool res1 = rni(v1, 1, true);

    if (res0 && !res1) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}
