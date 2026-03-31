/* gadget comes from the article
 * https://eprint.iacr.org/2026/004.pdf
*/

#include "verif_msi_pp.hpp"



int main(){

    Node & k1 = symbol("k1", 'S', 4);
    Node & k2 = symbol("k2", 'S', 4);
    Node & k3 = symbol("k3", 'S', 4);
    std::vector<Node *> a = getRealShares(k1, 2);
    std::vector<Node *> b = getRealShares(k2, 2);
    std::vector<Node *> c = getRealShares(k3, 2);

    Node & k10 = *a[0];
    Node & k11 = *a[1];
    Node & k20 = *b[0];
    Node & k21 = *b[1];
    Node & k30 = *c[0];
    Node & k31 = *c[1];
    Node & m0 = symbol("m0", 'M', 4);
    Node & m1 = symbol("m1", 'M', 4);
    Node & m2 = symbol("m2", 'M', 4);
    Node & m3 = symbol("m3", 'M', 4);
    Node & m4 = symbol("m4", 'M', 4);
    Node & m5 = symbol("m5", 'M', 4);
    Node & m6 = symbol("m6", 'M', 4);


    HWElement & x0 = inputGate(k10);
    HWElement & x1 = inputGate(k11);
    HWElement & y0 = inputGate(k20);
    HWElement & y1 = inputGate(k21);
    HWElement & z0 = inputGate(k30);
    HWElement & z1 = inputGate(k31);
    HWElement & r0 = inputGate(m0);
    HWElement & r1 = inputGate(m1);
    HWElement & r2 = inputGate(m2);
    HWElement & r3 = inputGate(m3);
    HWElement & r4 = inputGate(m4);
    HWElement & r5 = inputGate(m5);
    HWElement & r6 = inputGate(m6);

    HWElement & x1y1 = andGate(x1, y1);
    HWElement & x0y0 = andGate(x0, y0);
    HWElement & x1z1 = andGate(x1, z1);
    HWElement & x0z0 = andGate(x0, z0);
    HWElement & y1z1 = andGate(y1, z1);
    HWElement & y0z0 = andGate(y0, z0);
    HWElement & x1y1z1 = andGate(x1y1, z1);
    HWElement & x0y0z0 = andGate(x0y0, z0);

    HWElement & x1_p = xorGate(x1, r0);
    HWElement & y1_p = xorGate(y1, r1);
    HWElement & z1_p = xorGate(z1, r2);
    HWElement & x1y1_p = xorGate(x1y1, r3);
    HWElement & x1z1_p = xorGate(x1z1, r4);
    HWElement & y1z1_p = xorGate(y1z1, r5);
    HWElement & x1y1z1_p = xorGate(x1y1z1, r6);

    HWElement & x0r5 = andGate(x0, r5);
    HWElement & y0r4 = andGate(y0, r4);
    HWElement & z0r3 = andGate(z0, r3);
    HWElement & x0y0r2 = andGate(x0y0, r2);
    HWElement & x0z0r1 = andGate(x0z0, r1);
    HWElement & y0z0r0 = andGate(y0z0, r0);
    std::vector<HWElement *> vect_fc = {&x0y0z0, &x0r5, &y0r4, &z0r3, &x0y0r2, &x0z0r1, &y0z0r0, &r6};
    HWElement & fc = xorGate(vect_fc);

    HWElement & r_x0 = Register(x0);
    HWElement & r_y0 = Register(y0);
    HWElement & r_z0 = Register(z0);
    HWElement & r_x1y1_p = Register(x1y1_p);
    HWElement & r_x1z1_p = Register(x1z1_p);
    HWElement & r_y1z1_p = Register(y1z1_p);
    HWElement & r_x1_p = Register(x1_p);
    HWElement & r_y1_p = Register(y1_p);
    HWElement & r_z1_p = Register(z1_p);
    HWElement & r_fc = Register(fc);


    HWElement & x0_y1z1_p = andGate(r_x0, r_y1z1_p);
    HWElement & y0_x1z1_p = andGate(r_y0, r_x1z1_p);
    HWElement & z0_x1y1_p = andGate(r_z0, r_x1y1_p);
    HWElement & x0y0z1_p = andGate(andGate(r_x0, r_y0), r_z1_p);
    HWElement & x0z0y1_p = andGate(andGate(r_x0, r_z0), r_y1_p);
    HWElement & y0z0x1_p = andGate(andGate(r_y0, r_z0), r_x1_p);
    std::vector<HWElement *> vect_f0 = {&x0_y1z1_p, &y0_x1z1_p, &z0_x1y1_p, &x0y0z1_p, &x0z0y1_p, &y0z0x1_p, &r_fc};
    

    HWElement & f0 = xorGate(vect_f0);
    HWElement & f1 = Register(x1y1z1_p);
    
    

    std::vector<HWElement *> v {&f0, &f1};
    dumpCircuit("test_opti.dot", v);

    bool fonct = compareExpsWithExev(f0.getSymbExp() ^ f1.getSymbExp(), k1 & k2 & k3);
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
