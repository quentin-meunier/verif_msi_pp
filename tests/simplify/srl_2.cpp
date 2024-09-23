
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & s = symbol("s", 'P', 16);

    Node & c = constant(0x0, 16);

    Node & e = LShR(c, s);

    checkResults(e, c);

    return 0;
}

