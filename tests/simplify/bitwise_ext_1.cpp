
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k = symbol("k", 'S', 8);
    Node & p = symbol("p", 'P', 8);

    Node n = Extract(7, 0, ZeroExt(24, k) ^ ZeroExt(24, p) ^ ZeroExt(24, k));

    Node & res = p;

    checkResults(n, res);

    return 0;
}


