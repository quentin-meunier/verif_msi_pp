#include "verif_msi_pp.hpp"

// Permutation (checked on the content): T[m] can mask k
static uint8_t T[256];

int main() {
    for (int32_t i = 0; i < 256; i += 1) {
        T[i] = (uint8_t) (i * 7 + 3);
    }
    registerArray("T", 8, 8, 0x0, 256, NULL, T, 1);
    Node & k = symbol("k", 'S', 8);
    Node & m = symbol("m", 'M', 8);
    Node & n = k ^ getArrayByName("T")[m];
    checkTpsResult(n, true);
    return 0;
}
