
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);
    Node & m2 = symbol("m2", 'M', 8);
    Node & m3 = symbol("m3", 'M', 8);

    Node & n0 = k0 ^ m0 ^ m1;
    Node & n1 = n0 & m2;

    Node & n2 = m0 ^ m1 ^ k0;
    Node & n3 = n2 & m2;

    Node & n4 = n1 ^ n3 ^ m3;
    Node & n5 = n4 + k1;

    checkTpsResult(n5, true);



    return 0;
}


