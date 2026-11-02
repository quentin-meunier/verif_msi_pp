
#include "verif_msi_pp.hpp"


int main() {

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & m = symbol("m", 'M', 8);

    Node & n = ~(k0 ^ m) + k1;

    checkTpsResult(n, true);

    return 0;
}


