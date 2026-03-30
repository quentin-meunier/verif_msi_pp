/* gadget comes from the article
 * https://eprint.iacr.org/2026/004.pdf
*/

#include "verif_msi_pp.hpp"



int main(){

    Node & k1 = symbol("k1", 'S', 4);
    Node & k2 = symbol("k2", 'S', 4);
    std::vector<Node *> a = getRealShares(k1, 2);
    std::vector<Node *> b = getRealShares(k2, 2);

    Node & k10 = *a[0];
    Node & k11 = *a[1];
    Node & k20 = *b[0];
    Node & k21 = *b[1];
    Node & z0 = symbol("z0", 'M', 4);
    Node & z1 = symbol("z1", 'M', 4);
    Node & z2 = symbol("z2", 'M', 4);
    Node & z3 = symbol("z3", 'M', 4);
    Node & z4 = symbol("z4", 'M', 4);
    Node & z5 = symbol("z5", 'M', 4);


    HWElement & x0 = inputGate(k10);
    HWElement & x1 = inputGate(k11);
    HWElement & y0 = inputGate(k20);
    HWElement & y1 = inputGate(k21);
    HWElement & r0 = inputGate(z0);
    HWElement & r1 = inputGate(z1);
    HWElement & r2 = inputGate(z2);
    HWElement & r3 = inputGate(z3);
    HWElement & r4 = inputGate(z4);
    HWElement & r5 = inputGate(z5);

    HWElement & x1y1 = andGate(x1, y1);
    HWElement & x0y0 = andGate(x0, y0);
    HWElement & x0r2 = andGate(x0, r2);
    HWElement & y0r0 = andGate(y0, r0);
    HWElement & x0r3 = andGate(x0, r3);
    HWElement & y0r1 = andGate(y0, r1);
    
    std::vector<HWElement *> vect_x1_p = {&x1, &r0, &r1};
    HWElement & x1_p = xorGate(vect_x1_p);
    std::vector<HWElement *> vect_y1_p = {&y1, &r2, &r3};
    HWElement & y1_p = xorGate(vect_y1_p);
    std::vector<HWElement *> vect_xy1_p = {&x1y1, &r4, &r5};
    HWElement & xy1_p = xorGate(vect_xy1_p);
    std::vector<HWElement *> vect_fc1 = {&x0y0, &x0r2, &y0r0, &r4};
    HWElement & fc1 = xorGate(vect_fc1);
    std::vector<HWElement *> vect_fc2 = {&x0r3, &y0r1, &r5};
    HWElement & fc2 = xorGate(vect_fc2);

    HWElement & r_x0 = Register(x0);
    HWElement & r_y1_p = Register(y1_p);
    HWElement & r_y0 = Register(y0);
    HWElement & r_x1_p = Register(x1_p);
    HWElement & r_fc1 = Register(fc1);
    HWElement & r_fc2 = Register(fc2);
    
    
    HWElement & x0y1_p = andGate(r_x0, r_y1_p);
    HWElement & y0x1_p = andGate(r_y0, r_x1_p);
    std::vector<HWElement *> vect_f0 = {&x0y1_p, &y0x1_p, &r_fc1, &r_fc2};

    HWElement & f0 = xorGate(vect_f0);
    HWElement & f1 = Register(xy1_p);
    
    

    std::vector<HWElement *> v {&f0, &f1};
    dumpCircuit("test_opti.dot", v);

    bool fonct = compareExpsWithExev(f0.getSymbExp() ^ f1.getSymbExp(), k1 & k2);
    if (fonct) {
        std::cout << "# Functionality (exhaustive evaluation): [OK]" << std::endl;
    }
    else {
        std::cout << "# Functionality (exhaustive evaluation): [KO]" << std::endl;
    }


    int32_t order = 1;
    bool withGlitches = false;
    bool noFalsePositive = false;

    SecurityProperty sp = PINI;
    std::vector<HWElement *> outputList = {&f0, &f1};
    int32_t * nbCheck;

    int32_t res = checkSecurity(order, withGlitches, sp, outputList, noFalsePositive);

    std::cout << "res = ";
    std::cout<< res << std::endl;
    

    return 0;
}
