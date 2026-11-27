#include "verif_msi_pp.hpp"

// Content unknown, not declared: a[k ^ m] alone is still secure (replacement under the array)

int main() {
    ArrayExp a = ArrayExp("a", 8, 8);
    Node & k = symbol("k", 'S', 8);
    Node & m = symbol("m", 'M', 8);
    Node & n = a[k ^ m];
    checkTpsResult(n, true);
    return 0;
}
