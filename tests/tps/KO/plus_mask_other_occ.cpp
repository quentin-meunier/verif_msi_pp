#include "verif_msi_pp.hpp"


int main() {
    // The CTR k ^ (m2 + m) is replaced by m: the other occurrence of m must be
    // replaced by the inverse function (m ^ k) - m2, and not by the CTR itself
    // (which is only correct for an involution such as a chain of '^').
    // With the CTR, the expression became ((m2 + m) ^ m2) & m, without k.
    Node & k = symbol("k", 'S', 2);
    Node & m = symbol("m", 'M', 2);
    Node & m2 = symbol("m2", 'M', 2);

    Node & n = (k ^ (m2 + m)) & (k ^ m2 ^ m);

    checkTpsResult(n, false);

    return 0;
}
