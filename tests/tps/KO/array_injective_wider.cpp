#include "verif_msi_pp.hpp"

// Injective but not a permutation (8 -> 16 bits): T[m] is not uniform over 16 bits
static uint16_t T[256];

int main() {
    for (int32_t i = 0; i < 256; i += 1) {
        T[i] = (uint16_t) i;
    }
    registerArray("T", 8, 16, 0x0, 512, NULL, T, 2);
    Node & k = symbol("k", 'S', 16);
    Node & m = symbol("m", 'M', 8);
    Node & n = k ^ getArrayByName("T")[m];
    checkTpsResult(n, false);
    return 0;
}
