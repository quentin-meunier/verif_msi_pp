
#include "node.hpp"
#include "utils.hpp"
#include "arrayexp.hpp"


int main() {
    Node & p0 = symbol("p0", 'P', 3);
    Node & p1 = symbol("p1", 'P', 3);
    Node & p2 = symbol("p2", 'P', 3);
    ArrayExp a = ArrayExp("a", 3, 3);

    Node & f = p0 ^ p1 ^ p2 ^ p0;
    Node & n = a[f];

    Node & res = a[p1 ^ p2];

    checkResults(n, res);

    return 0;
}


