
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k = symbol("k", 'S', 8);
    Node & p = symbol("p", 'P', 8);

    Node & m0 = symbol("m0", 'M', 8);
    Node & p1 = symbol("p1", 'P', 8);
    Node & p2 = symbol("p2", 'P', 8);

    Node & n0 = k ^ m0;

    Node & n1 = p1 + n0;
    Node & n2 = n0 + p2;
    Node & n3 = ~n2;
    Node & n4 = n1 & n3;
    Node & n5 = ~n4;
    Node & n6 = m0 ^ k;
    Node & n7 = n6 + p;
    Node & n8 = n5 + n4 + n6 + n7;

    checkTpsResult(n8, true);

    return 0;
}


