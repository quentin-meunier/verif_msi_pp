
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    Node & n0 = ZeroExt(24, k);
    Node & n1 = Extract(7, 7, n0);

    Node & n2 = Extract(7, 7, k);

    checkResults(n1, n2);
    
    return 0;
}


