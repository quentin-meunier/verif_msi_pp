
#include "verif_msi_pp.hpp"


int main() {

    Node & s = symbol("p", 'P', 4);

    Node & c = constant(0xffff, 16);

    Node & e = c << s;

    Node & res = Concat(constant(1, 1), constant(0x7fff, 15) << s);

    checkResults(e, res);

    return 0;
}

