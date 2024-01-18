
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    Node & n0 = SignExt(24, k);
    Node & n1 = Extract(7, 0, n0);

    checkResults(n1, k);
    
    return 0;
}


