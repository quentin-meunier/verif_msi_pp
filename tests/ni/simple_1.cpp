
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    
    Node & m = symbol("m", 'M', 8);

    std::vector<Node *> a = getRealShares(k0, 3);
    std::vector<Node *> b = getRealShares(k1, 3);

    Node & a0 = *a[0];
    Node & a1 = *a[1];
    Node & a2 = *a[2];

    Node & b0 = *b[0];
    Node & b1 = *b[1];
    Node & b2 = *b[2];

    Node & e = a0 ^ a1 ^ b1 ^ b2;
    checkNIResult(e, 2, true);

    Node & n = e | (a0 & b1);
    checkNIResult(n, 2, true);

    Node & f = (a2 ^ m ^ b0);
    Node & g = Concat(n, f);
    checkNIResult(g, 2, true);

    return 0;
}


