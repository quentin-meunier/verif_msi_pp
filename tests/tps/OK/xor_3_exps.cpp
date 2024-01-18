
#include "node.hpp"
#include "utils.hpp"


int main() {

    // 3 exps:
    // m0 ^ k0
    // m0 ^ m1 ^ k1
    // m0 ^ m1 ^ m2 ^ k2

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    Node & k2 = symbol("k2", 'S', 8);

    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);
    Node & m2 = symbol("m2", 'M', 8);

    Node & n0 = k0 ^ m0;

    Node & n1 = k1 ^ m0;
    Node & n2 = n1 ^ m1;

    Node & n3 = k2 ^ m0;
    Node & n4 = n3 ^ m1;
    Node & n5 = n4 ^ m2;

    Node & e = Concat(n0, n2, n5);

    checkTpsResult(e, true);


    return 0;
}


