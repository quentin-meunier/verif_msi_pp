
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k = symbol("k", 'S', 8);
    Node & p = symbol("p", 'P', 8);
    Node & m = symbol("m", 'M', 8);

    Node & n = SignExt(24, k) ^ SignExt(24, p) ^ SignExt(24, m);

    Node & res = SignExt(24, k ^ p ^ m);


    checkResults(n, res, true, false);

    return 0;
}


