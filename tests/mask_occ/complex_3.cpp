
#include "verif_msi_pp.hpp"


int main() {
    Node & m = symbol("m", 'M', 8);
    Node & k = symbol("k", 'S', 8);
    Node & p = symbol("p", 'P', 8);

    ArrayExp a = ArrayExp("a", 8, 8);


    //           &
    //         /||\\
    //        / || \\
    //       /  || | \
    //      /  / | |  ~
    //     +-- | | |  |
    //    / \ \|/  |  |
    //   |  |  A  /   /
    //   p   \ | / __/
    //         ~__/
    //         |
    //         ^
    //        / \
    //       m   k


    Node & n0 = m ^ k;
    Node & n1 = ~n0;

    Node & n2 = a[n1];
    Node & n3 = Node::OpNode(PLUS, {&p, &n1, &n2});
    Node & n4 = ~n1;
    Node & n = Node::OpNode(BAND, {&n3, &n2, &n2, &n1, &n4});

    n.printMaskOcc();

    checkMaskOcc(
        n,
        {NULL, NULL},
        {},
        {
            {&m, {{&n0, {{&n0, {6, 0}}, {&n1, {6, 1}}, {&n2, {3, 2}}, {&n4, {1, 2}}}}}}
        },
        {}
    );

    n.dump("graph.dot");

    return 0;
}



