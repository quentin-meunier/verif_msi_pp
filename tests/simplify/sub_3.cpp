
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0, 8);
    Node & c1 = constant(0, 8);

    Node & n = c0 - c1;
    // result is 5
    Node & res = c0;

    checkResults(n, res);

    return 0;
}


