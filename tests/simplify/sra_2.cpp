
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & s = symbol("s", 'P', 16);

    Node & c = constant(0xffff, 16);

    Node & e = c >> s;

    checkResults(e, c);

    return 0;
}

