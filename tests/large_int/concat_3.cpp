
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0xAA, 992);
    Node & c1 = constant(0x12345678, 32);
    Node & n = Concat(c0, c1);
    Node & ref = constant(0xAA12345678, 1024);

    std::cout << "res: " << n << std::endl;
    std::cout << "ref: " << ref << std::endl;

    checkResults(n, ref);

    return 0;
}


