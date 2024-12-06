
#include "utils.hpp"

int main() {
    Node & m = symbol("m", 'P', 8);
    Node & k = symbol("k", 'P', 8);

    registerArray("sbox", 8, 8, 0x0, 256, NULL, nullptr, 1);
    ArrayExp & sbox = getArrayByName("sbox");

    Node & e = m ^ Concat(
        Extract(0x7, 0x7, m) ^ Extract(0x7, 0x7, sbox[k]),
        Extract(0x6, 0x6, m) ^ Extract(0x6, 0x6, sbox[k]),
        Extract(0x5, 0x5, m) ^ Extract(0x5, 0x5, sbox[k]),
        Extract(0x4, 0x4, m) ^ Extract(0x4, 0x4, sbox[k]),
        Extract(0x3, 0x3, m) ^ Extract(0x3, 0x3, sbox[k]),
        Extract(0x2, 0x2, m) ^ Extract(0x2, 0x2, sbox[k]),
        Extract(0x1, 0x1, m) ^ Extract(0x1, 0x1, sbox[k]),
        Extract(0x0, 0x0, m) ^ Extract(0x0, 0x0, sbox[k]));

    Node & res = sbox[k];

    checkResults(e, res);

    return 0;
}


