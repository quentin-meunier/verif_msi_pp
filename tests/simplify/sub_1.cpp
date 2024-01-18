
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 2);
    Node & c = constant(0, 2);

    Node & n = k - k;
    // result is k
    Node & res = c;


    checkResults(n, res);

    return 0;
}


