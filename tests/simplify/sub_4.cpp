
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0, 8);
    Node & c1 = constant(5, 8);
    Node & c2 = constant(251, 8);
    
    Node & n = c0 - c1;
    // result is -5 or 251
    Node & res = c2;

    checkResults(n, res);

    return 0;
}


