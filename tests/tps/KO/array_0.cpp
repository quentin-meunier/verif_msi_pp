
#include "node.hpp"
#include "utils.hpp"
#include "arrayexp.hpp"


int main() {
    Node & k = symbol("k", 'S', 3);
    Node & m = symbol("m", 'M', 3);

    ArrayExp a = ArrayExp("a", 3, 3);

    Node & n = m ^ a[k ^ m];

    checkTpsResult(n, false);

    return 0;
}


