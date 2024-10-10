
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & p = symbol("p", 'S', 3);
    Node & q = symbol("q", 'S', 3);

    Node & pb = getBitDecomposition(p);

    Node & n0 = p + q;

    Node & n1 = getBitDecomposition(n0);

    //std::cout << "n1: " << n1 << std::endl;

    Node & n2 = -q;
    Node & n3 = getBitDecomposition(n2);

    //std::cout << "n3: " << n3 << std::endl;

    Node & n4 = n1 + n3;
    Node & n = getBitDecomposition(n4);

    //std::cout << "n: " << n << std::endl;

    checkResults(n, pb);

    return 0;
}


