
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & p = symbol("p", 'P', 8);
    Node & q = symbol("q", 'P', 8);
    
    Node & c0 = constant(0x7F, 8);
    Node & c1 = constant(0xFE, 8);
    Node & c2 = constant(0xF7, 8);

    Node & n = c0 & p & c1 & q & c2;

    Node & res = p & q & constant(0x76, 8);

    checkResults(n, res);

    return 0;
}


