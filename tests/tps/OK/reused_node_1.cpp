
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 1);

    Node & m0 = symbol("m0", 'M', 1);
    Node & m1 = symbol("m1", 'M', 1);
    Node & m2 = symbol("m2", 'M', 1);
    Node & m3 = symbol("m3", 'M', 1);
    Node & m4 = symbol("m4", 'M', 1);

    Node & n0 = k0 ^ m0;

    Node & n1 = n0 & m1;
    Node & n2 = n0 & m2;
    Node & n3 = n0 & m3;

    Node & n4 = n1 ^ n2 ^ n3;


    checkTpsResult(n4, true);

    return 0;
}


