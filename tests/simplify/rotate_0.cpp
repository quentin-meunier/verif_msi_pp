
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0x12345678, 32);
    Node & n = RotateRight(c0, 8);

    Node & res = constant(0x78123456, 32);

    checkResults(n, res);

    return 0;
}


