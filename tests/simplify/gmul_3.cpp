
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);
 
    Node & n = GMul(a, c) ^ GMul(a, d) ^ GMul(b, c) ^ GMul(b, d);

    Node & res = GMul(a ^ b, c ^ d);

    checkResults(n, res);

    return 0;
}


