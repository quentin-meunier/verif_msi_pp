
#include "utils.hpp"


int main() {

    Node & c = constant(0xFFFFFFFFFFFFFFFF, 64);
    Node & e = c >> 8;
    Node & r = constant(0xFFFFFFFFFFFFFFFF, 64);

    checkResults(e, r);

    return 0;
}

