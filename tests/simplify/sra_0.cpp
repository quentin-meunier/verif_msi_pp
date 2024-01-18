
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & c = constant(0xFFFFFFFFFFFFFFFF, 63);
    Node & e = c >> 8;
    Node & r = constant(0xFFFFFFFFFFFFFFFF, 63);

    checkResults(e, r);


    return 0;
}

