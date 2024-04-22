
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x5555555555555555, 64);
    Node & c1 = Concat(c0, c0);
    Node & n = Extract(127, 0, c1);

    std::cout << "res: " << n << std::endl;
    std::cout << "ref: " << c1 << std::endl;

    checkResults(n, c1);

    return 0;
}


