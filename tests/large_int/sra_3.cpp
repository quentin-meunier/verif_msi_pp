
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & c0 = constant(0xef, 8);
    Node & r = c0 >> 4;
    Node & c1 = constant(0xfe, 8);

    checkResults(r, c1);

    return 0;
}


