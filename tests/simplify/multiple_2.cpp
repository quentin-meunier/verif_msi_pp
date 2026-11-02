
#include "verif_msi_pp.hpp"


int main() {
 
    Node & s0 = symbol("p0", 'S', 3);
    Node & s1 = symbol("p1", 'S', 3);
    Node & s2 = symbol("p2", 'S', 3);
    Node & s3 = symbol("p3", 'S', 3);

    Node & f = s0 & s1 & s2 & s3;
    Node & e = s0 ^ s0 ^ f;
    Node & n = getBitDecomposition((e << 2) & (LShR(e, 1)));

    //n.dump("graph.dot");
    
    checkResults(n, constant(0, 3));

    return 0;
}


