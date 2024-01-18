
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 1);

    Node & m0 = symbol("m0", 'M', 1);
    
    Node & p0 = symbol("p0", 'P', 1);
    Node & p1 = symbol("p1", 'P', 1);
    Node & p2 = symbol("p2", 'P', 1);

    Node & n0 = k0 ^ m0;

    Node & n1 = n0 & p0;
    Node & n2 = ~n0;
    Node & n3 = n0 | p2;
    Node & n  = n1 + n2 + n3;

    checkTpsResult(n, true);

    return 0;
}


