
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);
    Node & e = symbol("e", 'P', 8);
    Node & f = symbol("f", 'P', 8);
    
    Node & n = (~a & ~b & ~c & ~d & e) | (~a & ~b & ~c & d) | (~a & ~b & ~c & ~d & ~e & f) | (~a & ~b & c) | (~a & b) | a | ~f;

    Node & res = constant(0xff, 8);

    checkResults(n, res);

    return 0;
}


