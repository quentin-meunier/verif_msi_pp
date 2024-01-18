#include <iostream>

#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {
    Node & a = symbol("a", 'P', 4);
    
    Node & e = a - a;
    std::cout << getBitDecomposition(e) << std::endl;

    //checkResults(n, res);

    return 0;
}


