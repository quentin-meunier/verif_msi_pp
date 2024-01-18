
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k0 = symbol("k0", 'S', 3);
    Node & k1 = symbol("k1", 'S', 3);
 
    Node & p0 = symbol("p0", 'M', 3);
    Node & p1 = symbol("p1", 'M', 3);
    Node & p2 = symbol("p2", 'M', 3);
    Node & p3 = symbol("p3", 'M', 3);

    Node & n0 = k0 ^ p0 ^ p1;
    Node & n1 = n0 & p2;

    Node & n2 = p0 ^ p1 ^ k0;
    Node & n3 = n2 & p2;

    Node & n4 = n1 ^ n3 ^ p3;
    Node & n = n4 + k1;

    Node & res = k1 + p3;

    checkResults(n, res);

    return 0;
}


