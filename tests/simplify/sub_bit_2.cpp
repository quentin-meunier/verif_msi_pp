
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & p = symbol("p", 'S', 4);

    Node & pb = getBitDecomposition(p);

    Node & n = pb - pb;

    //std::cout << "n: " << n << std::endl;

    Node & res = constant(0, 4);

    checkResults(n, res);

    return 0;
}


