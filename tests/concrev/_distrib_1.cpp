
#include "utils.hpp"
#include "concrev.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & exp_0 = Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1)))) & SignExt(Const(7, 5), Extract(Const(0, 1), Const(0, 1), ((((ZeroExt(Const(24, 5), (m1 ^ k1)) >> Const(1, 1)) >> Const(1, 1)) >> Const(1, 1)) >> Const(1, 1))));
        
    Node & exp_1 = Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1)))))) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), Extract(Const(7, 3), Const(0, 1), ((SignExt(Const(31, 5), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 32)) ^ (ZeroExt(Const(24, 5), m0) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1))))) << Const(1, 1))));


    bool rud;

    bool sid = getDistribWithExev(exp_0, &rud);

    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    sid = getDistribWithExev(exp_1, &rud);

    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    sid = getDistribWithExev(exp_0 ^ exp_1, &rud);

    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    return 0;
}


