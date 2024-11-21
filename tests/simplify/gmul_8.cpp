
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);

    Node & n = GMul(GPow(a, constant(3, 8)), GPow(a, constant(2, 8)));

    Node & res = GPow(a, constant(5, 8));

    checkResults(n, res);

    return 0;
}


