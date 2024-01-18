
#include "node.hpp"
#include "utils.hpp"


int main() {
    
    Node & m0 = symbol("m0", 'M', 1);
    Node & m1 = symbol("m1", 'M', 1);
    Node & m2 = symbol("m2", 'M', 1);

    Node & k = symbol("k", 'S', 1);

    Node & e = ((m2 & k) ^ m1) & ((~m0 ^ m1) ^ k);

    checkTpsResult(e, true);

    return 0;
}


