
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);

    Node & n = Node::OpNode(GMUL, {&a, &a, &a, &a});

    Node & res = GPow(a, constant(4, 8));

    checkResults(n, res);

    return 0;
}


