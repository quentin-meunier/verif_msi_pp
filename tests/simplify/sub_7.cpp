
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 2);
    Node & q = symbol("q", 'P', 2);

    Node & n = -p + q;

    Node & res = q - p;

    checkResults(n, res);

    return 0;
}


