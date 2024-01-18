
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & p = symbol("p", 'P', 8);
    Node & p0 = Extract(0, 0, p);

    Node & n = Extract(6, 0, Concat(p0, p0, p0, p0, p0, p0, p0, p0));
    Node & res = Concat(p0, p0, p0, p0, p0, p0, p0);

    checkResults(n, res);
    
    return 0;
}


