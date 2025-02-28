
#include "utils.hpp"


int main() {
    Node & e = constant(0x180000000, 33);
    Node & c = constant(31, 5);

    Node & n = LShR(e, c);
    Node & r = constant(0x3, 33);

    checkResults(n, r);

    return 0;
}

