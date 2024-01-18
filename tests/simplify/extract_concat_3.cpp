
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & a = symbol("a", 'S', 9);
    Node & b = symbol("b", 'S', 7);
    Node & c = symbol("c", 'S', 10);
    Node & d = symbol("d", 'S', 6);
    Node & n0 = Concat(d, c, b, a);

    Node & n = Extract(15, 0, n0);
    Node & res = Concat(b, a);

    checkResults(n, res);
    
    return 0;
}


