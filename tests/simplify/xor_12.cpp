
#include "node.hpp"
#include "utils.hpp"


int main() {

    //    root       root
    //     ^          ~
    //    / \   ->    |
    //   m   1        m
    
    
    Node & k0 = symbol("k0", 'S', 8);
    Node & c0 = constant(255, 8);
    
    Node & n = k0 ^ c0;
    
    Node & res = ~k0;


    checkResults(n, res);

    return 0;
}


