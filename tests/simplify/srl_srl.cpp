
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 8);

    Node & n = LShR(LShR(LShR(p, 1), 2), 3) + LShR(LShR(LShR(LShR(p, 1), 0), 3), 1);

    Node & res = LShR(p, 6) + LShR(p, 5);

    checkResults(n, res);

    return 0;
}


