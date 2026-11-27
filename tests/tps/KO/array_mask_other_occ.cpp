#include "verif_msi_pp.hpp"


static uint8_t S[256];


int main() {
    // Same as plus_mask_other_occ with a permutation array: when k ^ S[m] is replaced by m,
    // the other occurrence of m must be replaced by S_inv[m ^ k] (inverse array computed
    // from the content of S), and not by k ^ S[m]
    for (int32_t i = 0; i < 256; i += 1) {
        S[i] = (uint8_t) (i * 7 + 3);
    }
    registerArray("S", 8, 8, 0x0, 256, NULL, S, 1);
    Node & k = symbol("k", 'S', 8);
    Node & m = symbol("m", 'M', 8);

    Node & n = (k ^ getArrayByName("S")[m]) & m;

    checkTpsResult(n, false);

    return 0;
}
