
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);

    Node & n = ZeroExt(24, k) ^ ZeroExt(24, k) ^ ZeroExt(24, constant(12, 8));

    Node & res = constant(12, 32);

    checkResults(n, res);

    return 0;
}


