
#include "verif_msi_pp.hpp"


int main() {
    Node & e = constant(0x180000000, 33);
    Node & c = constant(31, 5);
    Node & y = symbol("y", 'P', 5);

    Node & n = e >> (c | y - y);
    std::cout << "n : " << n << std::endl;
    Node & s = simplify(n);
    std::cout << "s : " << s << std::endl;
    Node & r = constant(0x1ffffffff, 33);

    checkResults(n, r);


    return 0;
}

