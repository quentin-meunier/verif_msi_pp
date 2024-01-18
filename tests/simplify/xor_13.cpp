
#include "node.hpp"
#include "utils.hpp"


int main() {

    //    root       root
    //     ^          ~
    //    / \   ->    |
    //   m   1        m
    
    
    Node & k0 = symbol("k0", 'S', 8);
    Node & c0 = constant(0, 8);
    
    Node & n = k0 ^ k0;
    
    Node & res = c0;


    checkResults(n, res);

    return 0;
}


