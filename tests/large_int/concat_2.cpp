
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x111111, 24);
    Node & c1 = constant(0x22222222, 32);
    Node & c2 = constant(0x33333333, 32);
    Node & c3 = constant(0x4444444444, 40);
    Node & n = Concat(c3, c2, c1, c0);

    std::cout << "res: " << n << std::endl;
    std::cout << "ref: 0x44444444443333333322222222111111" << std::endl;
    if (n.nlimbs == 2 && n.cst[1] == 0x4444444444333333 && n.cst[0] == 0x3322222222111111) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


