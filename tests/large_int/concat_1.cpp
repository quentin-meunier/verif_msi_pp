
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x11111111, 32);
    Node & c1 = constant(0x22222222, 32);
    Node & c2 = constant(0x33333333, 32);
    Node & c3 = constant(0x44444444, 32);
    Node & n = Concat(c3, c2, c1, c0);

    std::cout << "res: " << n << std::endl;
    std::cout << "ref: 0x44444444333333332222222211111111" << std::endl;
    if (n.nlimbs == 2 && n.cst[1] == 0x4444444433333333 && n.cst[0] == 0x2222222211111111) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


