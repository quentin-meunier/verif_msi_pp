
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 8);
    Node & e = ZeroExt(24, p);

    Node & n = Extract(30, 23, e);

    Node & res = constant(0, 8);
    
    checkResults(n, res);
    
    return 0;
}


