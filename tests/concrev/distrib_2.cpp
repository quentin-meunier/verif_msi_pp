
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & exp_0 = ((Concat(Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0)) & Const(27, 8)) ^ Concat(((Concat(Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0)) & Const(27, 7)) ^ Concat(((Concat(Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0)) & Const(27, 6)) ^ Concat(((Concat(Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0)) & Const(27, 5)) ^ Concat(Extract(Const(3, 2), Const(0, 1), m0), Const(0, 1))), Const(0, 1))), Const(0, 1))), Const(0, 1))) & SignExt(Const(7, 5), (Extract(Const(4, 3), Const(4, 3), m1) ^ Extract(Const(4, 3), Const(4, 3), k1)));


    Node & exp_1 = (Concat((Extract(Const(7, 3), Const(7, 3), m0) ^ Extract(Const(3, 2), Const(3, 2), m0)), (Extract(Const(7, 3), Const(7, 3), m0) ^ Extract(Const(3, 2), Const(3, 2), m0)), (Extract(Const(7, 3), Const(7, 3), m0) ^ Extract(Const(3, 2), Const(3, 2), m0)), (Extract(Const(7, 3), Const(7, 3), m0) ^ Extract(Const(3, 2), Const(3, 2), m0)), (Extract(Const(7, 3), Const(7, 3), m0) ^ Extract(Const(3, 2), Const(3, 2), m0)), (Extract(Const(7, 3), Const(7, 3), m0) ^ Extract(Const(3, 2), Const(3, 2), m0)), (Extract(Const(7, 3), Const(7, 3), m0) ^ Extract(Const(3, 2), Const(3, 2), m0)), (Extract(Const(7, 3), Const(7, 3), m0) ^ Extract(Const(3, 2), Const(3, 2), m0))) & Const(27, 8)) ^ Concat(((Concat(Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0), Extract(Const(4, 3), Const(4, 3), m0)) & Const(27, 7)) ^ Concat(((Concat(Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0), Extract(Const(5, 3), Const(5, 3), m0)) & Const(27, 6)) ^ Concat(((Concat(Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0), Extract(Const(6, 3), Const(6, 3), m0)) & Const(27, 5)) ^ Concat(((Concat(Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0), Extract(Const(7, 3), Const(7, 3), m0)) & Const(11, 4)) ^ Concat(Extract(Const(2, 2), Const(0, 1), m0), Const(0, 1))), Const(0, 1))), Const(0, 1))), Const(0, 1))), Const(0, 1));


    bool rud;

    //Node & es = simplify(exp_0, true, true);
    //std::cout << "es : " << es << std::endl;

    bool sid = getDistribWithExev(exp_0, &rud);

    if (!rud && sid) {
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


