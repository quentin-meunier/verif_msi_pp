
#include "node.hpp"
#include "utils.hpp"
#include "simplify.hpp"


int main() {
    Node & p = symbol("p", 'P', 8);
    Node & m = symbol("m", 'M', 1);

    Node & n = Extract(7, 0, ZeroExt(24, p) ^ ZeroExt(31, m));

    Node & res = Concat(constant(0, 7), m) ^ p;

    checkResults(n, res);

    return 0;
}


