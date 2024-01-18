
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & s0 = symbol("s0", 'S', 1);
    Node & r0 = symbol("r0", 'M', 1);

    Node & n0 = s0 ^ r0;
    Node & n1 = s0 ^ r0;
    Node & n2 = s0 ^ r0;
    Node & n3 = s0 ^ r0;
    Node & n4 = s0 ^ r0;
    Node & n5 = s0 ^ r0;
    Node & n6 = s0 ^ r0;
    Node & n7 = s0 ^ r0;

    Node & e = Concat(n0, n1, n2, n3, n4, n5, n6, n7);

    checkTpsResult(e, true);

    return 0;
}


