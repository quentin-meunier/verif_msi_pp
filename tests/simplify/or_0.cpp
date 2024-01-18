
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    Node & k2 = symbol("k2", 'S', 8);
 
    Node & n = k0 | k0 | k1 | k0 | k0 | k1 | k2 | k2 | k1 | k0;
    Node & res = k0 | k1 | k2;

    checkResults(n, res);

    return 0;
}


