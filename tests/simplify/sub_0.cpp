
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 3);
    Node & c = constant(0, 3);

    Node & n = k - c;
    // result is k
    Node & res = k;


    checkResults(n, res);

    return 0;
}


