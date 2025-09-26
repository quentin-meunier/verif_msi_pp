
#include "verif_msi_pp.hpp"


int main() {
    Node & m = symbol("m", 'M', 4);
    Node & n = symbol("n", 'M', 4);

    Node & p = symbol("p", 'P', 4);
    Node & q = symbol("q", 'P', 4);

    Node & k = symbol("k", 'S', 4);

    bool rud;
    bool sid;


    Node & e0 = k ^ m;
    sid = getDistribWithExev(e0, &rud);

    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }



    Node & e1 = k & m;
    sid = getDistribWithExev(e1, &rud);

    if (!rud && !sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }



    Node & e2 = k ^ p;
    sid = getDistribWithExev(e2, &rud);

    if (!rud && !sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }



    Node & e3 = p & (k ^ m);

    sid = getDistribWithExev(e3, &rud);
    if (!rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }



    Node & e4 = p | (k ^ m);

    sid = getDistribWithExev(e4, &rud);
    if (!rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }



    Node & e5 = m ^ p;

    sid = getDistribWithExev(e5, &rud);
    if (rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    Node & e6 = m & p;

    sid = getDistribWithExev(e6, &rud);
    if (!rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }



    Node & e7 = (m ^ p) & n;

    sid = getDistribWithExev(e7, &rud);
    if (!rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }


    Node & e8 = (m ^ p) & q;

    sid = getDistribWithExev(e8, &rud);
    if (!rud && sid) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


