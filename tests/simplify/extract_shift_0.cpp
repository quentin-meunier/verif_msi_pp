
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    Node & n = Extract(7, 7, k >> 1);

    Node & res = Extract(7, 7, k);
    
    checkResults(n, res);
    
    return 0;
}


