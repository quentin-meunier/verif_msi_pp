
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {
    Node & p = symbol("p", 'S', 8);

    Node & pb = getBitDecomposition(p);

    for (int i = 0; i < 1000; i += 1) {

        int32_t v = rand() % 256;
        std::cout << v << std::endl;

        Node & c0 = constant(v, 8);

        Node & n0 = pb + c0;

        //std::cout << "n0: " << n0 << std::endl;

        Node & n1 = getBitDecomposition(n0);

        //std::cout << "n1: " << n1 << std::endl;

        Node & c1 = constant(255 - v, 8);

        Node & n2 = n1 + c1;

        //std::cout << "n2: " << n2 << std::endl;

        Node & n = getBitDecomposition(n2);

        if (!equivalence(simplify(n), pb)) {

            std::cout << "KO: " << v << std::endl;
            return EXIT_FAILURE;
        }
    }
    std::cout << "OK" << std::endl;

    return EXIT_SUCCESS;
}


