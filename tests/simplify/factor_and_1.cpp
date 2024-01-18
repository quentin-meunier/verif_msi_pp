
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    
    Node & n = a & b | a;
    Node & res = a;

    checkResults(n, res);

    return 0;
}


