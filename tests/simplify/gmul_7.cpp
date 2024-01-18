
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);

    Node & n = GMul(GMul(a, constant(1, 8)), constant(5, 8));

    Node & res = GMul(a, constant(5, 8));

    checkResults(n, res);

    return 0;
}


