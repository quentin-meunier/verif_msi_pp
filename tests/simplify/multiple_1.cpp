
#include "node.hpp"
#include "utils.hpp"


int main() {
 
    Node & a = symbol("a", 'S', 2);
    Node & b = symbol("b", 'S', 2);
    Node & c = symbol("c", 'S', 2);
    Node & d = symbol("d", 'S', 2);
    Node & f = symbol("f", 'S', 2);
    Node & z = constant(0, 2);

    Node & n1 = a ^ b ^ c ^ a ^ b; // c
    Node & n2 = c ^ c;             // 0
    Node & n3 = z + c + n2;        // c
    Node & n4 = n1 & n3 & c;       // c

    Node & n5 = c ^ d ^ f ^ d;     // c ^ f
    Node & n6 = a ^ a;             // 0
    Node & n7 = n5 + n6;           // c ^ f
    Node & n8 = c ^ f;             // c ^ f
    Node & n9 = c ^ f;             // c ^ f
    Node & n10 = n8 & n9;          // c ^ f
    Node & n11 = n7 | n10;         // c ^ f
    Node & n = n4 ^ n11;           // f
    
    // Result is f
    Node & res = f;

    checkResults(n, res);

    return 0;
}


