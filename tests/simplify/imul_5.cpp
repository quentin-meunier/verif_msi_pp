
#include "node.hpp"
#include "utils.hpp"


int main() {
    Node & a = symbol("a", 'P', 8);
    Node & b = symbol("b", 'P', 8);
    Node & c = symbol("c", 'P', 8);
    Node & z = symbol("z", 'P', 8);
 
    Node & n = (a * z) + (z * (a + b)) + c;
    Node & res = z * (a + a + b) + c;

    checkResults(n, res);

    return 0;
}


