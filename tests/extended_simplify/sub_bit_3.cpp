
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {
    Node & p = symbol("p", 'S', 8);

    Node & pb = getBitDecomposition(p);

    Node & c0 = constant(0xff, 8);

    Node & n0 = pb + c0;

    //std::cout << "n0: " << n0 << std::endl;

    Node & n1 = getBitDecomposition(n0);

    //std::cout << "n1: " << n1 << std::endl;

    Node & c1 = constant(1, 8);

    Node & n2 = n1 + c1;

    //std::cout << "n2: " << n2 << std::endl;

    Node & n = getBitDecomposition(n2);

    checkResults(n, pb);

    return 0;
}


