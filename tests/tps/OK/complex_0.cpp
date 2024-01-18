
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'S', 1);
    Node & b = symbol("b", 'S', 1);
    
    Node & a2 = symbol("a2", 'M', 1);
    Node & a3 = symbol("a3", 'M', 1);
    Node & b2 = symbol("b2", 'M', 1);
    Node & b3 = symbol("b3", 'M', 1);

    Node & z0_4 = symbol("z0_4", 'M', 1);
    Node & z1_4 = symbol("z1_4", 'M', 1);
    Node & z2_4 = symbol("z2_4", 'M', 1);
    Node & z3_4 = symbol("z3_4", 'M', 1);


    Node & e0 = Concat(a2, z3_4,
               ((b ^ b3) & a3) ^ z1_4 ^ z2_4 ^ z3_4 ^ z0_4 ^ (a & b3),
               z1_4 ^ ((a2 ^ a) & b3) ^ z2_4 ^ z0_4 ^ ((b ^ b2 ^ b3) & a3));

    checkTpsResult(e0, true);


    Node & e1 = ((b ^ b3) & a3) ^ (a & b3) ^ ((a2 ^ a) & b3) ^ (b2 & a3);

    checkTpsResult(e1, true);


    Node & e2 = Concat(((b ^ b3) & a3) ^ z1_4 ^ z2_4 ^ z3_4 ^ z0_4 ^ (a & b3),
               (a2 & b3) ^ z3_4,
               z1_4 ^ ((a2 ^ a) & b3) ^ z2_4 ^ z0_4 ^ ((b ^ b2 ^ b3) & a3),
               z3_4);

    checkTpsResult(e2, true);



    return 0;
}


