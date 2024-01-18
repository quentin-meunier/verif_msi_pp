
#include "node.hpp"
#include "utils.hpp"


int main() {

    //      /        /
    //     ^        /  
    //    / \   -> /
    //   ~a  1    a

    Node & k = symbol("k", 'S', 8);
    Node & p = symbol("p", 'S', 8);
    
    Node & c = constant(255, 8);
    
    Node & n0 = ~k ^ c;
    Node & n = n0 & p;
    
    Node & res = k & p;


    checkResults(n, res);

    return 0;
}


