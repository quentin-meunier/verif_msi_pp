
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {

    Node & k = symbol("k", 'S', 32);

    Node & n0 = k ^ k;
    Node & n1 = k ^ k;

    if (equivalence(n0, n1)) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


