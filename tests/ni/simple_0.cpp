
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    
    Node & m = symbol("m", 'M', 8);

    std::vector<Node *> u = getRealShares(k0, 3);
    std::vector<Node *> v = getRealShares(k1, 3);

    Node & u0 = *u[0];
    Node & u1 = *u[1];
    Node & u2 = *u[2];

    Node & v0 = *v[0];
    Node & v1 = *v[2];
    Node & v2 = *v[2];

    Node & e = u0 ^ m ^ u1 ^ u2;
    //std::cout << "e : " << e << std::endl;
    checkNIResult(e, 2, true);

    e = u0 ^ v0 ^ u1 ^ u2;
    checkNIResult(e, 2, false);

    return 0;
}


