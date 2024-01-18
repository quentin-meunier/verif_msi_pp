
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & s0 = symbol("s0", 'S', 1);
    Node & s1 = symbol("s1", 'S', 1);
    Node & s2 = symbol("s2", 'S', 1);
    Node & s3 = symbol("s3", 'S', 1);
    Node & s5 = symbol("s5", 'S', 1);
    Node & s6 = symbol("s6", 'S', 1);
    Node & s7 = symbol("s7", 'S', 1);

    Node & r0 = symbol("r0", 'M', 1);
    Node & r1 = symbol("r1", 'M', 1);
    Node & r2 = symbol("r2", 'M', 1);
    Node & r3 = symbol("r3", 'M', 1);
    Node & r4 = symbol("r4", 'M', 1);
    Node & r5 = symbol("r5", 'M', 1);
    Node & r6 = symbol("r6", 'M', 1);
    Node & r7 = symbol("r7", 'M', 1);

    Node & n0 = s7 ^ r0;
    Node & n1 = s1 ^ r6 ^ s2 ^ r5 ^ s7 ^ r0;
    Node & n2 = s5 ^ r2 ^ s6 ^ r1 ^ s7 ^ r0 ^ s1 ^ r6 ^ r3;
    Node & n3 = r3 ^ s6 ^ r1 ^ s7 ^ r0 ^ s0 ^ r7 ^ s3 ^ r4;
    Node & n4 = r6 ^ r5 ^ r4 ^ r0;
    Node & n5 = r6 ^ r5 ^ r1 ^ r0;
    Node & n6 = r7 ^ r6 ^ r5 ^ r0;
    Node & n7 = r7 ^ r6 ^ r5 ^ r2 ^ r1 ^ r0;

    Node & e = Concat(n0, n1, n2, n3, n4, n5, n6, n7);


    checkTpsResult(e, true);

    return 0;
}


