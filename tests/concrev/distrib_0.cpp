
#include "verif_msi_pp.hpp"


int main() {
    Node & m0 = symbol("m0", 'M', 4);
    Node & m1 = symbol("m1", 'M', 4);
    Node & m2 = symbol("m2", 'M', 2);
    Node & k0 = symbol("k0", 'S', 4);

    Node & e0 = k0 ^ m0;
    bool rud;
    bool sid = getDistribWithExev(e0, &rud);

    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    Node & e1 = k0 & m0;

    sid = getDistribWithExev(e1, &rud);
    if (!rud && !sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    Node & e2 = k0 ^ m0 ^ m1;

    sid = getDistribWithExev(e2, &rud);
    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    Node & e3 = k0 ^ m0 ^ (ZeroExt(2, m2));

    sid = getDistribWithExev(e3, &rud);
    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    Node & e4 = (k0 ^ m0) & m1;

    sid = getDistribWithExev(e4, &rud);
    if (!rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    // if using e0 instead of using a new alias, strange bug
    Node & e5 = k0;

    sid = getDistribWithExev(e5, &rud);
    if (!rud && !sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


