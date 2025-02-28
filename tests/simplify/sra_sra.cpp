
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 8);

    Node & n = (((p >> 1) >> 2) >> 3) + ((((p >> 1) >> 0) >> 3) >> 1);

    Node & res = (p >> 6) + (p >> 5);

    checkResults(n, res);

    return 0;
}


