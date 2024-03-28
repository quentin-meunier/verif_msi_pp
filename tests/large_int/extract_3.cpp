
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x5555555555555555, 64);
    Node & n = Extract(63, 0, c0);

    std::cout << "res: " << n << std::endl;
    std::cout << "ref: 0x5555555555555555" << std::endl;
    if (n.nlimbs == 1 && n.cst[0] == 0x5555555555555555) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


