
#include "concrev.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    Node & m = symbol("m", 'M', 8);
    Node & m0 = simplify(Extract(0, 0, m));

    std::vector<Node *> shares = getRealShares(k, 2);
    Node & k0 = *shares[0];
    Node & k1 = *shares[1];
    Node & k0b = getBitDecomposition(k0);
    Node & k1b = getBitDecomposition(k1);
    Node & k0b0 = simplify(Extract(0, 0, k0b));
    Node & k1b0 = simplify(Extract(0, 0, k1b));

    Node & kb = getBitDecomposition(k);
    std::vector<Node *> shares0 = getRealShares(simplify(Extract(0, 0, kb)), 2);
    Node & kb0 = *shares0[0];
    Node & kb1 = *shares0[1];

    std::cout << "k    : " << k.verbatimPrint() << std::endl;
    std::cout << "k0   : " << k0.verbatimPrint() << std::endl;
    std::cout << "k0b  : " << k0b.verbatimPrint() << std::endl;
    std::cout << "k1   : " << k1.verbatimPrint() << std::endl;
    std::cout << "k1b  : " << k1b.verbatimPrint() << std::endl;
    std::cout << "k0b0 : " << k0b0.verbatimPrint() << std::endl;
    std::cout << "k1b0 : " << k1b0.verbatimPrint() << std::endl;
    std::cout << "kb0  : " << kb0.verbatimPrint() << std::endl;
    std::cout << "kb1  : " << kb1.verbatimPrint() << std::endl;


    bool r = compareExpsWithRandev(k0b0 ^ m0, kb0 ^ m0, 100);

    if (r) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    r = compareExpsWithExev(k0b0 ^ m0, kb0 ^ m0);

    if (r) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    r = compareExpsWithRandev(k0b0, kb0, 100);

    if (r) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    r = compareExpsWithExev(k0b0, kb0);

    if (r) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    bool rud;
    bool sid = getDistribWithExev(k0b0, &rud);
    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    sid = getDistribWithExev(kb0, &rud);
    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}

