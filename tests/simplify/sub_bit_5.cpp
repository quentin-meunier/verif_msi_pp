
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & p = symbol("p", 'S', 4);

    Node & pb = getBitDecomposition(p);
    Node & npb = getBitDecomposition(-p);

    //std::cout << "pb :  " << pb << std::endl;
    //std::cout << "npb:  " << npb << std::endl;

    Node & n = getBitDecomposition(pb + npb);

    //std::cout << "n:    " << n << std::endl;

    Node & res = constant(0, 4);

    checkResults(n, res);

    return 0;
}


