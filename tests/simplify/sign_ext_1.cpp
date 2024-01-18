
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & c = constant(0xFE, 8);
    Node & e = SignExt(56, c);
    Node & r = constant(0xFFFFFFFFFFFFFFFE, 64);

    checkResults(e, r);


    return 0;
}

