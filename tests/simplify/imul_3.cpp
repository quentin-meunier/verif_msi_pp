
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);
 
    Node & n = a * c + a * d + b * c + b * d;
    Node & res = (a + b) * (c + d);

    checkResults(n, res);

    return 0;
}


