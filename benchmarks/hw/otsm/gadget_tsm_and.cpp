

#include "verif_msi_pp.hpp"



int main(){

    Node & k1 = symbol("k1", 'S', 2);
    Node & k2 = symbol("k2", 'S', 2);
    std::vector<Node *> a = getRealShares(k1, 2);
    std::vector<Node *> b = getRealShares(k2, 2);

    Node & k10 = *a[0];
    Node & k11 = *a[1];
    Node & k20 = *b[0];
    Node & k21 = *b[1];
    Node & z0 = symbol("z0", 'M', 2);
    Node & z1 = symbol("z1", 'M', 2);
    Node & z2 = symbol("z2", 'M', 2);
    Node & z3 = symbol("z3", 'M', 2);
    Node & z4 = symbol("z4", 'M', 2);

    HWElement & x0 = inputGate(k10);
    HWElement & x1 = inputGate(k11);
    HWElement & y0 = inputGate(k20);
    HWElement & y1 = inputGate(k21);
    HWElement & r0 = inputGate(z0);
    HWElement & r1 = inputGate(z1);
    HWElement & r2 = inputGate(z2);
    HWElement & r3 = inputGate(z3);
    HWElement & r4 = inputGate(z4);
    
    HWElement & x0_p = xorGate(x0, r3);
    HWElement & x1_p = xorGate(x1, r3);
    HWElement & y0_p = xorGate(y0, r4);
    HWElement & y1_p = xorGate(y1, r4);

    HWElement & r_r0 = Register(r0);
    HWElement & r_r1 = Register(r1);
    HWElement & r_r2 = Register(r2);
    HWElement & r_y1_p = Register(y1_p);
    HWElement & r_x1_p = Register(x1_p);

    HWElement & x0y0_p = andGate(x0_p, y0_p);
    HWElement & r0x0y0_p = xorGate(x0y0_p, r0);
    HWElement & r_r0x0y0_p = Register(r0x0y0_p);
    HWElement & r_r1x0_p = Register(xorGate(x0_p, r1));
    HWElement & r_r2y0_p = Register(xorGate(y0_p, r2));
    HWElement & r1x0y1_p = andGate(r_r1x0_p, r_y1_p);
    HWElement & r2y0x1_p = andGate(r_r2y0_p, r_x1_p);
    std::vector<HWElement *> vect_f0 = {&r_r0x0y0_p, &r1x0y1_p, &r2y0x1_p};
    
    HWElement & r1y1_p = andGate(r_r1, r_y1_p);
    HWElement & r2x1_p = andGate(r_r2, r_x1_p);
    HWElement & x1y1_p = andGate(r_x1_p, r_y1_p);
    std::vector<HWElement *> vect_f1 = {&r_r0, &r1y1_p, &r2x1_p, &x1y1_p};

    HWElement & f0 = xorGate(vect_f0);
    HWElement & f1 = xorGate(vect_f1);
    
    

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
