
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);
    Node & u = symbol("u", 'P', 8);
    Node & v = symbol("v", 'P', 8);
    Node & z = symbol("z", 'P', 8);
    Node & w = symbol("w", 'P', 8);
 
    Node & n = a * (u * (v + z) + d) + a * (d + u * (w + z));
    Node & res = a * (u * (v + w + z + z) + d + d);

    checkResults(n, res);

    return 0;
}


