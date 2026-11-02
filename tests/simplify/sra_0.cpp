
#include "verif_msi_pp.hpp"


int main() {

    Node & c = constant(0xFFFFFFFFFFFFFFFF, 63);
    Node & e = c >> 8;
    Node & r = constant(0xFFFFFFFFFFFFFFFF, 63);

    checkResults(e, r);


    return 0;
}

