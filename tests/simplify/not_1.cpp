
#include "node.hpp"
#include "utils.hpp"

//  &       &
//  |       |
//  ~   ->  |
//  |       |
//  ~       |
//  |       |
//  k       k



int main() {

    Node & k0 = symbol("k0", 'S', 8);

    Node & n0 = ~k0;
    Node & n = ~n0;

    Node & res = k0;

    checkResults(n, res);

    return 0;
}


