
#include "node.hpp"
#include "utils.hpp"
#include "check_leakage.hpp"


int main() {
    
    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & m1 = symbol("m1", 'M', 8);

    Node & e0 = ZeroExt(24, k0);
    Node & e1 = SignExt(31, (Extract(0, 0, m1) ^ Extract(0, 0, k1)));


    bool res = checkTpsTransXorBit(e0, e1);

    if (res) {
        std::cout << "OK" << std::endl;
    }
    else {
        std::cout << "KO" << std::endl;
    }

    return 0;
}


