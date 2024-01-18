
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 8);
    Node & e = SignExt(24, p);
    Node & n = Extract(30, 23, e);

    Node & res = SignExt(7, Extract(7, 7, p));
    
    checkResults(n, res);
    
    return 0;
}


