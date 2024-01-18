
#include "node.hpp"
#include "utils.hpp"
#include "arrayexp.hpp"


int main() {
    Node & s = symbol("s", 'S', 1);
    Node & t = symbol("t", 'S', 1);
    Node & u = symbol("u", 'S', 1);

    Node & r = symbol("r", 'M', 1);

    Node & n = (s ^ r ^ t) & u;

    checkTpsResult(n, false);

    return 0;
}


