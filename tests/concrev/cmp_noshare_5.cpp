
#include "concrev.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & m = symbol("m", 'M', 8);

    Node & e0 = Concat(m + m, constant(0, 56));
    Node & e1 = Concat(constant(2, 8) * m, constant(0, 56));

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

