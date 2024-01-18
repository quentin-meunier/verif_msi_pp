
#include "node.hpp"
#include "utils.hpp"


int main() {

    // (3 | (e ^ e)) ^ (4 | (f ^ f))

    Node & p = symbol("p", 'P', 4);
    Node & q = symbol("q", 'P', 4);

    Node & c0 = constant(15, 4);
    Node & c1 = constant(12, 4);

    Node & n = (c0 | (p ^ p)) ^ (c1 | (q ^ q));

    Node & res = constant(3, 4);


    checkResults(n, res);

    return 0;
}


