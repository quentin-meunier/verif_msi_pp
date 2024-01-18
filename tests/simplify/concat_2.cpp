
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & a = symbol("a", 'P', 8);
    Node & c1 = constant(0x56, 8);
    Node & c2 = constant(0x34, 8);
    Node & c3 = constant(0x12, 8);
    Node & c4 = constant(0x123456, 24);
    Node & c5 = constant(0x563412, 24);

    Node & n = Concat(c3, c2, c1, a, c1, c2, c3, a, c3, c2, c1);
    Node & res = Concat(c4, a, c5, a, c4);

    checkResults(n, res);

    return 0;
}


