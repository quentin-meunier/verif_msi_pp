
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    Node & c = constant(0, 7);
    Node & d = constant(0, 10);

    Node & n1 = Extract(7, 0, Concat(d, c, c, k));

    checkResults(n1, k);
    
    return 0;
}


