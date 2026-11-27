#include "verif_msi_pp.hpp"

// An access to a non-injective array does not preserve the mask
static uint8_t T[256];

int main() {
    for (int32_t i = 0; i < 256; i += 1) {
        T[i] = i & 1;
    }
    registerArray("T", 8, 8, 0x0, 256, NULL, T, 1);
    Node & k = symbol("k", 'S', 8);
    Node & m = symbol("m", 'M', 8);
    Node & n = k ^ getArrayByName("T")[m];
    checkTpsResult(n, false);
    return 0;
}
