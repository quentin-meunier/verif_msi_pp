
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & c0 = constant(-2, 8);
    Node & c1 = constant(2, 8);
    Node & k0 = symbol("k0", 'S', 8);

    Node & n0 = c0 + c1;
    Node & n1 = k0 | n0;

    checkResults(n1, k0);

    delete &c0;
    delete &c1;
    delete &k0;
    delete &n0;

    return 0;
}


