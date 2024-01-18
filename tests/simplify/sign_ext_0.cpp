
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & c = constant(0xFE, 8);
    Node & e = SignExt(55, c);
    Node & r = constant(0xFFFFFFFFFFFFFFFE, 63);

    checkResults(e, r);


    return 0;
}

