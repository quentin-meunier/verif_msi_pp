
#include "node.hpp"
#include "utils.hpp"


int main() {
 
    Node & p0 = symbol("p0", 'S', 3);
    Node & p1 = symbol("p1", 'S', 3);
    Node & p2 = symbol("p2", 'S', 3);
    Node & p3 = symbol("p3", 'S', 3);

    Node & f = p0 & p1 & p2 & p3;
    Node & e = p0 ^ p0 ^ f;
    Node & n = ~e | (e >> 1);

    //bool res = checkTpsVal(n);
    n.dump("graph.dot");

    return 0;
}


