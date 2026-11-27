
#include "verif_msi_pp.hpp"


int main() {
    // ~Concat(a, b) ^ Concat(a, b): the '~' removed from the children of the '^'
    // must be kept when the '^' is transformed into a Concat by mergeConcatChildren
    Node & a = symbol("a", 'P', 1);
    Node & b = symbol("b", 'P', 1);

    Node & n = ~Concat(a, b) ^ Concat(a, b);
    Node & res = constant(3, 2);

    checkResults(n, res);

    return 0;
}

