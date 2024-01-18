
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & c0 = constant(-2, 8);
    Node & c1 = constant(2, 8);
    Node & c2 = constant(4, 8);
    Node & c3 = constant(4, 8);
    Node & c4 = constant(0, 8);

    Node & k0 = symbol("k0", 'S', 8);

    // n0 = (c0 + c1 + c2 + c3) ^ k0 ^ (c2 + c3) ^ k0
    Node & n0 = c0 + c1;
    Node & n1 = n0 + c2;
    Node & n2 = n1 + c3;
    Node & n3 = n2 ^ k0;
    Node & n4 = c2 + c3;
    Node & n5 = n3 ^ n4;
    Node & n = n5 ^ k0;

    checkResults(n, c4);

    delete &c0;
    delete &c1;
    delete &c2;
    delete &c4;

    delete &k0;

    delete &n2;
    delete &n3;
    delete &n5;
    delete &n;

    return 0;
}


