
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    Node & k2 = symbol("k2", 'S', 8);
    Node & c0 = constant(255, 8);
    Node & c1 = constant(0, 8);

    //Node & n = c0 & k0 & k0 & c0 & c0 & k1 & c0 & k0 & k0 & k1 & k2 & c0 & k2 & c0 & k1 & k0 & c0
    Node & n0 = c0 & k0;
    Node & n1 = n0 & k0;
    Node & n2 = n1 & c0;
    Node & n3 = n2 & c0;
    Node & n4 = n3 & k1;
    Node & n5 = n4 & c0;
    Node & n6 = n5 & k0;
    Node & n7 = n6 & k0;
    Node & n8 = n7 & k1;
    Node & n9 = n8 & k2;
    Node & n10 = n9 & c0;
    Node & n11 = n10 & k2;
    Node & n12 = n11 & c0;
    Node & n13 = n12 & k1;
    Node & n14 = n13 & k0;
    Node & n15 = n14 & c0;
    Node & n16 = n15 & c1;

    checkResults(n16, c1);

    delete &n0;
    delete &n1;
    delete &n2;
    delete &n3;
    delete &n4;
    delete &n5;
    delete &n6;
    delete &n7;
    delete &n9;
    delete &n10;
    delete &n11;
    delete &n12;
    delete &n13;
    delete &n14;
    delete &n15;
    delete &n16;

    delete &c0;
    delete &c1;

    delete &k0;
    delete &k1;
    delete &k2;

    return 0;
}


