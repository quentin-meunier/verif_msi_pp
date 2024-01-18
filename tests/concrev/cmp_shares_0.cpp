
#include "concrev.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    std::vector<Node *> shares = getRealShares(k, 2);
    Node & m0 = *shares[0];
    Node & m1 = *shares[1];

    Node & e0 = ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1))))) & SignExt(Const(31, 5), Extract(Const(0, 1), Const(0, 1), ((((ZeroExt(Const(24, 5), (m1 ^ k)) >> Const(1, 1)) >> Const(1, 1)) >> Const(1, 1)) >> Const(1, 1))));
        
    Node & e1 = ZeroExt(Const(24, 5), ((Concat(Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0)) & Const(27, 8)) ^ Concat(((Concat(Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0)) & Const(27, 7)) ^ Concat(((Concat(Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0)) & Const(27, 6)) ^ Concat(((Concat(Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 5)) ^ Concat(Extract(Const(3, 2), Const(0, 1), m0), Const(0, 1))), Const(0, 1))), Const(0, 1))), Const(0, 1)))) & SignExt(Const(31, 5), (Extract(Const(4, 3), Const(4, 3), m1) ^ Extract(Const(4, 3), Const(4, 3), k)));


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

