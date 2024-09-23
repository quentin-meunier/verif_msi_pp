
#include "concrev.hpp"
#include "utils.hpp"


int main() {

    Node & s = symbol("s", 'P', 16);

    Node & c = constant(0x1, 16);

    Node & e1 = s + c - s;
    Node & e2 = LShR(LShR(s, c), e1);

    //std::cout << "e2: " << e2.verbatimPrint() << std::endl;

    Node & r = LShR(s, constant(0x2, 16));

    checkResults(e2, r);

    return 0;
}


