
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & s = symbol("s", 'P', 16);

    Node & c = constant(0x1, 16);

    Node & e = s + c - s - c;

    Node & n = LShR(s, e);

    checkResults(n, n);

    return 0;
}


