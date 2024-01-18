
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & a = symbol("a", 'S', 7);
    Node & b = symbol("b", 'S', 9);
    Node & c = symbol("c", 'S', 10);
    Node & d = symbol("d", 'S', 6);
    Node & n0 = Concat(d, c, b, a);

    Node & n = Extract(26, 7, n0);
    Node & res = Extract(19, 0, Concat(d, c, b));

    checkResults(n, res);
    
    return 0;
}


