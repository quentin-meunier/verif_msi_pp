
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & a = symbol("a", 'S', 7);
    Node & b = symbol("b", 'S', 9);
    Node & c = symbol("c", 'S', 10);
    Node & d = symbol("d", 'S', 6);
    Node & n0 = Concat(d, c, b, a);

    Node & n = Extract(24, 10, n0);
    Node & res = Extract(17, 3, Concat(c, b));

    checkResults(n, res);
    
    return 0;
}


