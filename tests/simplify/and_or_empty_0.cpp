
#include "verif_msi_pp.hpp"


int main() {
    // ~d & c & (d | ~c) -> 0
    // Both operands of the '|' are 0 when the other operands of the '&' are all ones:
    // the '|' is then 0 (not all ones)
    Node & c = symbol("c", 'P', 8);
    Node & d = symbol("d", 'P', 8);

    Node & n = (~d & c) & (d | ~c);
    Node & res = constant(0, 8);

    checkResults(n, res);

    return 0;
}

