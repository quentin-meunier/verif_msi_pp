
#include "node.hpp"
#include "utils.hpp"


int main() {

    //    root       root
    //     ^          m
    //    /|\   ->
    //   k m k

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);

    Node & n = k0 ^ k1 ^ k0;

    Node & res = k1;

    checkResults(n, res);

    return 0;
}


