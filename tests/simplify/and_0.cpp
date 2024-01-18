
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    Node & k2 = symbol("k2", 'S', 8);

    //Node & n = k0 & k0 & k1 & k0 & k0 & k1 & k2 & k2 & k1 & k0;
    Node & n0 = k0 & k0;
    Node & n1 = n0 & k1;
    Node & n2 = n1 & k0;
    Node & n3 = n2 & k0;
    Node & n4 = n3 & k1;
    Node & n5 = n4 & k2;
    Node & n6 = n5 & k2;
    Node & n7 = n6 & k1;
    Node & n8 = n7 & k0;

    Node & r0 = k0 & k1;
    Node & res = r0 & k2;

    checkResults(n8, res);

    delete &n0;
    delete &n1;
    delete &n2;
    delete &n3;
    delete &n4;
    delete &n5;
    delete &n6;
    delete &n7;
    delete &n8;

    delete &r0;
    delete &res;

    delete &k0;
    delete &k1;
    delete &k2;

    return 0;
}


