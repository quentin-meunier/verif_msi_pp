
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);
    Node & e = symbol("e", 'P', 8);
    
    Node & n = (~a & b) | (~a & ~b & c) | (~a & ~b & ~c & d) | (a + b + c) | (~a & ~b & ~c & ~d & e) | a;
    Node & res = b | c | d | e | (a + b + c) | a;

    checkResults(n, res);

    return 0;
}


