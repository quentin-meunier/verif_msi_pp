
#include "node.hpp"
#include "utils.hpp"
#include "arrayexp.hpp"


int main() {
    Node & s = symbol("s", 'S', 1);
    Node & r = symbol("r", 'M', 1);

    Node & n = s ^ r ^ r;

    checkTpsResult(n, false);

    return 0;
}


