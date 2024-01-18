
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    Node & c = constant(0, 7);
    Node & d = constant(0, 10);
    Node & n0 = Concat(k, c, c, d);

    Node & n1 = Extract(31, 24, n0);

    checkResults(n1, k);
    
    return 0;
}


