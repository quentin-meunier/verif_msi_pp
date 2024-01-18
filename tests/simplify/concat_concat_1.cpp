
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);

    Node & n = Concat(a, Concat(Concat(d, b), c, Concat(Concat(a, b), b, Concat(c, d))), d, Concat(a, d));

    Node & res = Concat(a, d, b, c, a, b, b, c, d, d, a, d);

    checkResults(n, res);

    return 0;
}


