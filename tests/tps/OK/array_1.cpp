
#include "verif_msi_pp.hpp"


int main() {
    Node & k = symbol("k", 'S', 3);
    Node & m = symbol("m", 'M', 3);
    Node & p = symbol("p", 'P', 3);

    ArrayExp a = ArrayExp("a", 3, 3);

    Node & n = m ^ a[((k ^ m) << 1) ^ (m << 1) ^ p];

    checkTpsResult(n, true);

    return 0;
}


