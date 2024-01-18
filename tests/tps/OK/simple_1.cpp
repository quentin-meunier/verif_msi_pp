
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    
    Node & m = symbol("m", 'M', 8);

    std::vector<Node *> u = getPseudoShares(k0, 3);
    std::vector<Node *> v = getPseudoShares(k1, 3);

    Node & a0 = *u[0];
    Node & a1 = *u[1];
    Node & a2 = *u[2];

    Node & b0 = *v[0];
    Node & b1 = *v[2];
    Node & b2 = *v[2];

    Node & e = a0 ^ a1 ^ b1 ^ b2;

    Node & p = e | (a0 & b1);
    checkTpsResult(p, true);

    //std::cout << "e : " << e << std::endl;

    Node & f = (a2 ^ m ^ b0);
    Node & g = Concat(p, f);
    checkTpsResult(g, true);

    return 0;
}


