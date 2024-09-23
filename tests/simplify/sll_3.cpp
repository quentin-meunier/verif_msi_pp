
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & s = symbol("s", 'P', 16);

    Node & c = constant(0x1, 16);

    Node & e = s + c - s;

    Node & n = s << e;

    Node & r = s << c;

    checkResults(n, r);

    return 0;
}

