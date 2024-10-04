
#include "node.hpp"
#include "utils.hpp"

int main() {
    Node & p = symbol("b", 'P', 16);
    Node & c0 = constant(0xffffffff, 32);
    Node & e1 = Concat(constant(0x1000, 13), p, constant(0x7, 3));
    Node & n = e1 & c0;
    Node & res = Concat(constant(0x1000, 13), p, constant(0x7, 3));

    checkResults(n, res);
    return 0;
}

