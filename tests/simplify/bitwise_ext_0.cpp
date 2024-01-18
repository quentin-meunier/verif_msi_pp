
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & k = symbol("k", 'S', 8);
    Node & p = symbol("p", 'P', 8);
    Node & m = symbol("m", 'M', 8);

    Node & n0 = Extract(7, 0, ZeroExt(24, k) ^ ZeroExt(24, p) ^ ZeroExt(24, m));
    Node & n1 = Extract(7, 0, SignExt(24, k) ^ SignExt(24, p) ^ SignExt(24, m));
    Node & n = n0 ^ n1;

    Node & res = constant(0, 8);

    checkResults(n, res);

    return 0;
}


