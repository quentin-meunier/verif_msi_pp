
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & s = symbol("p", 'P', 4);

    Node & c = constant(0xffff, 16);

    Node & e = LShR(c, s);

    Node & res = Concat(LShR(constant(0x7fff, 15), s), constant(1, 1));

    checkResults(e, res);

    return 0;
}

