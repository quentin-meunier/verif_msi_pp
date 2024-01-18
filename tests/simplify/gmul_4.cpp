
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);
    Node & u = symbol("u", 'P', 8);
    Node & v = symbol("v", 'P', 8);
    Node & w = symbol("w", 'P', 8);
    Node & z = symbol("z", 'P', 8);
 
    Node & n = GMul(a, GMul(u, (v ^ z)) ^ d) ^ GMul(a, d ^ GMul(u, (w ^ z)));

    Node & res = GMul(a, u, v ^ w);

    checkResults(n, res);

    return 0;
}


