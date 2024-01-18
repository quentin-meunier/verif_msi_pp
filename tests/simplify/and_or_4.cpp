
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & ab = a & b;

    Node & n = Node::OpNode(BOR, {&ab, &a, &c});
    Node & res = a | c;

    checkResults(n, res);

    return 0;
}


