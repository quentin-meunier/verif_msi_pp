
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & c0 = constant(0, 8);
    Node & c1 = constant(0, 8);

    Node & n0 = c0 + c1;

    checkResults(n0, c0);

    delete &c0;

    return 0;
}


