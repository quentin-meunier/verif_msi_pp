
#include "concrev.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & m0 = symbol("m0", 'M', 8);

    Node & e0 = Extract(Const(7, 3), Const(0, 1), (ZeroExt(Const(24, 5), m0) << Const(1, 1)));
    Node & e1 = Concat(Extract(6, 0, m0), Const(0, 1));

    bool r = compareExpsWithRandev(e0, e1, 10000);

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

    r = compareExpsWithRandev(be0, be1, 10000);

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

