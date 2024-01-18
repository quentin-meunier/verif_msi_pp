
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 32);
    Node & n = Extract(2, 1, Extract(15, 8, p));

    Node & res = Extract(10, 9, p);
    
    checkResults(n, res);
    
    return 0;
}


