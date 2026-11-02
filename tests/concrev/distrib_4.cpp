
#include "verif_msi_pp.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    std::vector<Node *> shares = getRealShares(k, 2);
    Node & k0 = *shares[0];
    Node & k1 = *shares[1];
    //std::cout << *k0.symb << std::endl;

    Node & e0 = k ^ k0;
    //std::cout << e0.verbatimPrint() << std::endl;

    bool rud;
    bool sid = getDistribWithExev(replaceSharesWithSecretsAndMasks(e0), &rud);

    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    Node & e1 = k & k0;

    sid = getDistribWithExev(replaceSharesWithSecretsAndMasks(e1), &rud);

    if (!rud && !sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    return 0;
}

