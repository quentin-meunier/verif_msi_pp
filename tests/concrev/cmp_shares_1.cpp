
#include "concrev.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);

    std::vector<Node *> shares = getRealShares(k, 3);
    Node & a0 = *shares[0];
    Node & a1 = *shares[1];
    Node & a2 = *shares[2];

    Node & e0 = a0 ^ a1 ^ constant(0x34, 8) ^ a2;
    Node & e1 = k ^ constant(0x34, 8);


    bool r = compareExpsWithRandev(e0, e1, 100);

    if (r) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    r = compareExpsWithExev(e0, e1);

    if (r) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    Node & be0 = getBitDecomposition(e0);
    Node & be1 = getBitDecomposition(e1);

    r = compareExpsWithRandev(be0, be1, 100);

    if (r) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    r = compareExpsWithExev(be0, be1);

    if (r) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    return 0;
}

