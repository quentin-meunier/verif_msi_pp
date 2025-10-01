
#include "verif_msi_pp.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);

    Node & e0 = m0 ^ k0 | (m1 & k1);
    //std::cout << "e0 : " << e0 << std::endl;
    checkTpsResult(e0, false);

    bool res;
    res = getDistribWithExev(e0);
    std::cout << "# Res : " << res << std::endl;

    res = tpsNoFalsePositive(e0);
    std::cout << "# Res : " << res << std::endl;

    return 0;
}


