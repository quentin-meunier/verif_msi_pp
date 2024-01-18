
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 1);
    Node & n = Extract(7, 0, SignExt(31, p));

    Node & res = SignExt(7, p);
    
    checkResults(n, res);
    
    return 0;
}


