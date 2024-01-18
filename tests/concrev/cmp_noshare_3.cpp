
#include "concrev.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & m = symbol("m", 'M', 8);

    Node & e0 = Concat((m + m) << 1 ^ m & (-m + ~m), ZeroExt(7, Extract(0, 0, m)), SignExt(7, Extract(1, 1, m)), m >> 1, LShR(m, 2));
    Node & e1 = simplify(e0);

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

