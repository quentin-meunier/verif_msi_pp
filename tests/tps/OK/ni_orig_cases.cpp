
#include "node.hpp"
#include "utils.hpp"


int main() {

    Node & k0 = symbol("k0", 'S', 8);
    Node & k1 = symbol("k1", 'S', 8);
    Node & k2 = symbol("k2", 'S', 8);

    Node & m0 = symbol("m0", 'M', 8);
    Node & m1 = symbol("m1", 'M', 8);
    Node & m2 = symbol("m2", 'M', 8);

    Node & p0 = symbol("p0", 'P', 8);
    Node & p1 = symbol("p1", 'P', 8);
    Node & p2 = symbol("p2", 'P', 8);


    Node & n0 = (m0 & k0);
    checkTpsResult(n0, false);

    Node & n1 = (~m0 & k0);
    checkTpsResult(n1, false);

    Node & n2 = (~~m0 & k0);
    checkTpsResult(n2, false);

    Node & n3 = (~~~m0 & k0);
    checkTpsResult(n3, false);

    Node & n4 = (k0 & (p0 ^ m0));
    checkTpsResult(n4, false);

    Node & n5 = (k0 & (p0 ^ ~m0));
    checkTpsResult(n5, false);

    Node & n6 = (k0 & (p0 ^ ~~m0));
    checkTpsResult(n6, false);



    Node & n7 = (m0 ^ k0);
    checkTpsResult(n7, true);

    Node & n8 = (~m0 ^ k0);
    checkTpsResult(n8, true);

    Node & n9 = (~~m0 ^ k0);
    checkTpsResult(n9, true);

    Node & n10 = (~~~m0 ^ k0);
    checkTpsResult(n10, true);

    Node & n11 = (k0 ^ (p0 ^ m0));
    checkTpsResult(n11, true);

    Node & n12 = (k0 ^ (p0 ^ ~m0));
    checkTpsResult(n12, true);

    Node & n13 = (k0 ^ (p0 ^ ~~m0));
    checkTpsResult(n13, true);



    Node & n14 = (m1 ^ k1) & (m0 ^ k0);
    checkTpsResult(n14, true);

    Node & n15 = (m1 ^ k1) & (~m0 ^ k0);
    checkTpsResult(n15, true);

    Node & n16 = (m1 ^ k1) & (~~m0 ^ k0);
    checkTpsResult(n16, true);

    Node & n17 = (m1 ^ k1) & (~~~m0 ^ k0);
    checkTpsResult(n17, true);

    Node & n18 = (m1 ^ k1) & (k0 ^ (p0 ^ m0));
    checkTpsResult(n18, true);

    Node & n19 = (m1 ^ k1) & (k0 ^ (p0 ^ ~m0));
    checkTpsResult(n19, true);

    Node & n20 = (m1 ^ k1) & (k0 ^ (p0 ^ ~~m0));
    checkTpsResult(n20, true);



    Node & n21 = ((m0 ^ k0) & p0) & k2;
    checkTpsResult(n21, false);

    Node & n22 = (~((m0 ^ k0) + (k1 & m1)) & p1) ^ (m1 ^ k2);
    checkTpsResult(n22, true);

    Node & n23 = (~((m0 ^ k0) + (k1 & m0)) & p1) ^ (m0 ^ k2);
    checkTpsResult(n23, false);

    Node & n24 = (~((m0 ^ k0) + (k1 ^ m1)) & p1) ^ (m0 ^ k0);
    checkTpsResult(n24, true);


    return 0;
}


