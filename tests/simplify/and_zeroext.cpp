
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k = symbol("k", 'S', 8);
    Node & p = symbol("p", 'P', 8);
    Node & m = symbol("m", 'M', 8);

    Node & n = ZeroExt(24, k) & ZeroExt(24, p) & ZeroExt(24, m);

    Node & res = ZeroExt(24, k & p & m);

    checkResults(n, res);

    return 0;
}


