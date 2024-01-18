
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(6, 8);
    Node & c1 = constant(1, 8);
    Node & c2 = constant(5, 8);

    Node & n = c0 - c1;
    // result is 5
    Node & res = c2;

    checkResults(n, res);

    return 0;
}


