
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x5, 4);
    Node & c1 = constant(0x0, 60);
    Node & n = Concat(c0, c1);

    std::cout << "res: " << n << std::endl;
    std::cout << "ref: 0x5000000000000000" << std::endl;
    if (n.nlimbs == 1 && n.cst[0] == 0x5000000000000000) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


