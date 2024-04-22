
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x0, 992);
    Node & c1 = constant(0x0, 32);
    Node & n = Concat(c0, c1);

    std::cout << "res: " << n << std::endl;
    std::cout << "ref: 0x0" << std::endl;
    if (n.nlimbs == 16 && n.cst[1] == 0x0) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


