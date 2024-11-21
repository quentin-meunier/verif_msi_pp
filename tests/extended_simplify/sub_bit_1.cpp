
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {
    Node & p = symbol("p", 'S', 4);

    Node & pb = getBitDecomposition(p);

    Node & n0 = -p;

    //std::cout << "n0: " << n0 << std::endl;

    Node & n1 = -n0;

    std::cout << "n1: " << n1 << std::endl;

    Node & n = getBitDecomposition(n1);

    //std::cout << "n: " << n << std::endl;

    checkResults(n, getBitDecomposition(p));

    return 0;
}


