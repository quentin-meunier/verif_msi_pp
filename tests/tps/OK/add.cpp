
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k0 = symbol("k0", 'S', 8);

    Node & m = symbol("m", 'M', 8);

    Node & n = k0 + m;
    std::cout << "# Mask Occ in node: " << n << std::endl;
    n.printMaskOcc();

    checkTpsResult(n, true);

    return 0;
}


