#include "verif_msi_pp.hpp"

// Content unknown, declared as a permutation by the user

int main() {
    ArrayExp a = ArrayExp("a", 8, 8, true);
    Node & k = symbol("k", 'S', 8);
    Node & k2 = symbol("k2", 'S', 8);
    Node & m = symbol("m", 'M', 8);
    Node & n = k2 ^ a[k ^ m];
    checkTpsResult(n, true);
    return 0;
}
