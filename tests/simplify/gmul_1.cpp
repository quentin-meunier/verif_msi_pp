
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
 
    Node & n = GMul(a, b) ^ a;

    Node & res = GMul(a, (b ^ constant(0x01, 8)));

    checkResults(n, res);

    return 0;
}


