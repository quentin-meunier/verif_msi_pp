/* Copyright (C) 2026, Sorbonne Universite, LIP6
 * This file is part of the VerifMSI++ project, under the GPL v3.0 license
 * See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
 * SPDX-License-Identifier: GPL-3.0-only
 * Author: Quentin L. Meunier, Lucie Chauvière
 */

#include <cstring>
#include <iomanip>

#include "verif_msi_pp.hpp"


bool use_litteral_values = true;

int32_t nbCheck = 0;
int32_t nbLeak = 0;

void checkExpLeakage(Node & e) {
    nbCheck ++;
    bool res = checkTpsVal(e);
    if (!res) {
        nbLeak ++;
        std::cout << "# Leakage in value for exp num " << nbCheck << ", Node " << e << std::endl;
    }
}


uint8_t sbox[] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

uint8_t sboxp[256];


uint8_t rcon[10] = { 1, 2, 4, 8, 16, 32, 64, 128, 27, 54 };


Node & sboxp_func(Node & idx) {
    ArrayExp & sbox = getArrayByName("sbox");
    Node & m = getSymbolByName("m");
    Node & mp = getSymbolByName("mp");
    return simplify(sbox[idx ^ m] ^ mp);
}


void sub_bytes(Node * x[4][4]) {
    ArrayExp & sbox = getArrayByName("sbox");
    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            Node & e = simplify(sbox[*x[i][j]]);
            checkExpLeakage(e);
            x[i][j] = &e;
        }
    }
}


void masked_sub_bytes(Node * x[4][4]) {
    ArrayExp &  sboxp = getArrayByName("sboxp");
    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            Node & e = simplify(sboxp[*x[i][j]]);
            checkExpLeakage(e);
            x[i][j] = &e;
        }
    }
}


void shift_rows(Node * x[4][4]) {
    Node * tmp = x[1][0];
    x[1][0] = x[1][1];
    x[1][1] = x[1][2];
    x[1][2] = x[1][3];
    x[1][3] = tmp;

    tmp = x[2][0];
    x[2][0] = x[2][2];
    x[2][2] = tmp;
    tmp = x[2][1];
    x[2][1] = x[2][3];
    x[2][3] = tmp;
    
    tmp = x[3][0];
    x[3][0] = x[3][3];
    x[3][3] = x[3][2];
    x[3][2] = x[3][1];
    x[3][1] = tmp;
}

void mix_columns(Node * x[4][4]) {
    std::function<Node &(Node & y)> xtime;
    xtime = [&](Node & y) -> Node & {
        Node & e0 = simplify(y >> 7);
        checkExpLeakage(e0);
        Node & e1 = simplify(e0 & constant(0x1b, 8));
        checkExpLeakage(e1);
        Node & f = simplify(y << 1);
        checkExpLeakage(f);
        return simplify(f ^ e1);
    };
    
    for (int i = 0; i < 4; i += 1) {
        Node * t = x[0][i];
        // tmp    = x[0][i] ^ x[1][i] ^ x[2][i] ^ x[3][i]
        Node & e01 = simplify(*x[0][i] ^ *x[1][i]);
        checkExpLeakage(e01);
        Node & e02 = simplify(e01 ^ *x[2][i]);
        checkExpLeakage(e02);
        Node & e03 = simplify(e02 ^ *x[3][i]);
        checkExpLeakage(e03);
        Node & tmp = e03;

        Node & tm0 = simplify(*x[0][i] ^ *x[1][i]);
        checkExpLeakage(tm0);
        Node & tm1 = xtime(tm0);

        // x[0][i] ^= tm ^ tmp
        Node & e11 = simplify(tm1 ^ tmp);
        checkExpLeakage(e11);
        Node & e12 = simplify(*x[0][i] ^ e11);
        checkExpLeakage(e12);
        x[0][i] = &e12;
        
        Node & tm2 = simplify(*x[1][i] ^ *x[2][i]);
        checkExpLeakage(tm2);
        Node & tm3 = xtime(tm2);

        // x[1][i] ^= tm ^ tmp
        Node & e21 = simplify(tm3 ^ tmp);
        checkExpLeakage(e21);
        Node & e22 = simplify(*x[1][i] ^ e21);
        checkExpLeakage(e22);
        x[1][i] = &e22;
        
        Node & tm4 = simplify(*x[2][i] ^ *x[3][i]);
        checkExpLeakage(tm4);
        Node & tm5 = xtime(tm4);

        // x[2][i] ^= tm ^ tmp
        Node & e31 = simplify(tm5 ^ tmp);
        checkExpLeakage(e31);
        Node & e32 = simplify(*x[2][i] ^ e31);
        checkExpLeakage(e32);
        x[2][i] = &e32;
        
        Node & tm6 = simplify(*x[3][i] ^ *t);
        checkExpLeakage(tm6);
        Node & tm7 = xtime(tm6);

        // x[3][i] ^= tm ^ tmp
        Node & e41 = simplify(tm7 ^ tmp);
        checkExpLeakage(e41);
        Node & e42 = simplify(*x[3][i] ^ e41);
        checkExpLeakage(e42);
        x[3][i] = &e42;
    }
}


void mix_column(Node * x[4]) {
    Node * a[4];
    Node * b[4];
    for (int c = 0; c < 4; c += 1) {
        Node & tmp = simplify(*x[c]);
        checkExpLeakage(tmp);
        a[c] = &tmp;
        Node & h = simplify(*x[c] >> 7);
        checkExpLeakage(h);
        Node & post_b = simplify(*x[c] << 1);
        checkExpLeakage(post_b);
        // b[c] = post_b ^ constant(0x1b, 8) & h
        Node & e = simplify(constant(0x1B, 8) & h);
        checkExpLeakage(e);
        b[c] = &simplify(post_b ^ e);
        checkExpLeakage(*b[c]);
    }
    
    // x[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]
    Node & x01 = simplify(*b[0] ^ *a[3]);
    checkExpLeakage(x01);
    Node & x02 = simplify(x01 ^ *a[2]);
    checkExpLeakage(x02);
    Node & x03 = simplify(x02 ^ *b[1]);
    checkExpLeakage(x03);
    Node & x04 = simplify(x03 ^ *a[1]);
    checkExpLeakage(x04);
    x[0] = &x04;

    // x[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]
    Node & x11 = simplify(*b[1] ^ *a[0]);
    checkExpLeakage(x11);
    Node & x12 = simplify(x11 ^ *a[3]);
    checkExpLeakage(x12);
    Node & x13 = simplify(x12 ^ *b[2]);
    checkExpLeakage(x13);
    Node & x14 = simplify(x13 ^ *a[2]);
    checkExpLeakage(x14);
    x[1] = &x14;

    // x[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]
    Node & x21 = simplify(*b[2] ^ *a[1]);
    checkExpLeakage(x21);
    Node & x22 = simplify(x21 ^ *a[0]);
    checkExpLeakage(x22);
    Node & x23 = simplify(x22 ^ *b[3]);
    checkExpLeakage(x23);
    Node & x24 = simplify(x23 ^ *a[3]);
    checkExpLeakage(x24);
    x[2] = &x24;

    // x[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]
    Node & x31 = simplify(*b[3] ^ *a[2]);
    checkExpLeakage(x31);
    Node & x32 = simplify(x31 ^ *a[1]);
    checkExpLeakage(x32);
    Node & x33 = simplify(x32 ^ *b[0]);
    checkExpLeakage(x33);
    Node & x34 = simplify(x33 ^ *a[0]);
    checkExpLeakage(x34);
    x[3] = &x34;
}


void add_round_key(Node * x[4][4], Node * round_key[176], int rnd) {
    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            Node & e  = simplify(*round_key[(rnd << 4) + (i << 2) + j]);
            checkExpLeakage(e);
            Node & f = simplify(*x[j][i] ^ e);
            checkExpLeakage(f);
            x[j][i] = &f;
        }
    }
}


void add_round_key_no_verif(Node * x[4][4], Node * round_key[176], int rnd) {
    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            Node & e  = simplify(*round_key[(rnd << 4) + (i << 2) + j]);
            Node & f = simplify(*x[j][i] ^ e);
            x[j][i] = &f;
        }
    }
}


void masked_init_masked_aes_keys(Node * key[16], Node * round_key[176], Node & m, Node & mp, Node * mpt[4]) {
    ArrayExp &  sboxp = getArrayByName("sboxp");
    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            // round_key[i * 4 + j] = key[i * 4 + j] ^ mpt[j] ^ m
            Node & e0 = simplify(*key[i * 4 + j] ^ *mpt[j]);
            checkExpLeakage(e0);
            Node & e1 = simplify(e0 ^ m);
            checkExpLeakage(e1);
            round_key[i * 4 + j] = &e1;
        }
    }
    
    for (int i = 4; i < 40; i += 1) {
        // Generating the keys for rounds 1 to 9
        // 1 word per iteration
        // All keys are masked with M and (M1', M2', M3', M4')
        Node * temp[4];
        for (int j = 0; j < 4; j += 1) {
            temp[j] = round_key[(i - 1) * 4 + j];
        }

        if (i % 4 == 0) {
            Node & k = simplify(*temp[0] ^ *mpt[0]);
            checkExpLeakage(k);
            temp[0] = &simplify(*temp[1] ^ *mpt[1]);
            checkExpLeakage(*temp[0]);
            temp[1] = &simplify(*temp[2] ^ *mpt[2]);
            checkExpLeakage(*temp[1]);
            temp[2] = &simplify(*temp[3] ^ *mpt[3]);
            checkExpLeakage(*temp[2]);
            temp[3] = &k;

            // Masked with M
            for (int j = 0; j < 4; j += 1) {
                temp[j] = &simplify(sboxp[*temp[j]]);
                checkExpLeakage(*temp[j]);
            }
            // Masked with M'
            temp[0] = &simplify(*temp[0] ^ constant(rcon[(i / 4) - 1], 8)),
            checkExpLeakage(*temp[0]);

            // xoring with previous words adds masks M and (M1', M2', M3', M4')
            // We remask with M' to remove it
            for (int j = 0; j < 4; j +=1) {
                // round_key[i * 4 + j] = (round_key[(i - 4) * 4 + j] ^ temp[j]) ^ mp
                Node & e = simplify(*round_key[(i - 4) * 4 + j] ^ *temp[j]);
                checkExpLeakage(e);
                Node & f = simplify(e ^ mp);
                checkExpLeakage(f);
                round_key[i * 4 + j] = &f;
            }
            // Masked with M and (M1', M2', M3', M4')
        }
        else {
            // As both previous word and the same word in previous key are masked with
            // M and (M1', M2', M3', M4'), we remove M from the word in previous key
            // and (M1', M2', M3', M4') from the previous word before xoring
            for (int j = 0; j < 4; j +=1) {
                // round_key[i * 4 + j] = (round_key[(i - 4) * 4 + j] ^ m) ^ (temp[j] ^ mpt[j])
                Node & e = simplify(*round_key[(i - 4) * 4 + j] ^ m);
                checkExpLeakage(e);
                Node & f = simplify(*temp[j] ^ *mpt[j]);
                checkExpLeakage(f);
                Node & g = simplify(e ^ f);
                checkExpLeakage(g);
                round_key[i * 4 + j] = &g;
            }
        // Masked with M and (M1', M2', M3', M4')
        }        
    }

    for (int i = 40; i < 44; i += 1) {
        // For the last key, we mask it with M'
        Node * temp[4];
        for (int j = 0; j < 4; j += 1) {
            temp[j] = round_key[(i - 1) * 4 + j];
        }
        
        if (i % 4 == 0) {
            Node & k = simplify(*temp[0] ^ *mpt[0]);
            checkExpLeakage(k);
            temp[0] = &simplify(*temp[1] ^ *mpt[1]);
            checkExpLeakage(*temp[0]);
            temp[1] = &simplify(*temp[2] ^ *mpt[2]);
            checkExpLeakage(*temp[1]);
            temp[2] = &simplify(*temp[3] ^ *mpt[3]);
            checkExpLeakage(*temp[2]);
            temp[3] = &k;
        
            // Masked with M
            for (int j = 0; j < 4; j += 1) {
                temp[j] = &simplify(sboxp[*temp[j]]);
                checkExpLeakage(*temp[j]);
            }
            // Masked with M'
            temp[0] = &simplify(*temp[0] ^ constant(rcon[(i / 4) - 1], 8)),
            checkExpLeakage(*temp[0]);

            // xoring with previous words adds masks M and (M1', M2', M3', M4')
            // We remask with M' to remove it
            for (int j = 0; j < 4; j +=1) {
                // round_key[i * 4 + j] = (round_key[(i - 4) * 4 + j] ^ temp[j]) ^ mp
                Node & e0 = simplify(*round_key[(i - 4) * 4 + j] ^ *temp[j]);
                checkExpLeakage(e0);
                Node & e1 = simplify(e0 ^ m);
                checkExpLeakage(e1);
                Node  & e2 = simplify(e1 ^ *mpt[j]);
                checkExpLeakage(e2);
                round_key[i * 4 + j] = &e2;
            }
            // Masked with M'
        }
        else {
            // The previous word is masked with M' and the same word in previous key is masked with
            // M and (M1', M2', M3', M4'), we remove M and (M1', M2', M3', M4')
            for ( int j = 0; j < 4; j += 1){
                // round_key[i * 4 + j] = (round_key[(i - 4) * 4 + j] ^ temp[j]) ^ m ^ mpt[j]
                Node & e0 = simplify(*round_key[(i - 4) * 4 + j] ^ *temp[j]);
                checkExpLeakage(e0);
                Node & e1 = simplify(e0 ^ m);
                checkExpLeakage(e1);
                Node & e2 = simplify(e1 ^ *mpt[j]);
                checkExpLeakage(e2);
                round_key[i * 4 + j] = &e2;
            }
            // Masked with M'
        }
    }
}


// Mask all bytes from line i with mpt[i]
void mask_plain_text(Node * x[4][4], Node * mpt[4]) {
    for (int i = 0; i < 4; i += 1) {
        x[i][0] = &(*x[i][0] ^ *mpt[i]);
        x[i][1] = &(*x[i][1] ^ *mpt[i]);
        x[i][2] = &(*x[i][2] ^ *mpt[i]);
        x[i][3] = &(*x[i][3] ^ *mpt[i]);
    }
}


// Changes masks:
// - from M' to M1 on the first row
// - from M' to M2 on the second row
// - from M' to M3 on the third row
// - from M' to M4 on the forth row
void change_masks(Node * x[4][4], Node & mp, Node ** mt) {
    for (int i =0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            // x[i][j] = (x[i][j] ^ mt[i]) ^ mp 
            // order is important: masking with new mask before demasking old mask
            Node & e = simplify(*x[i][j] ^ *mt[i]);
            checkExpLeakage(e);
            Node & f = simplify(e ^ mp);
            checkExpLeakage(f);
            x[i][j] = &f;
        }
    }
}


void aes_herbst(Node * key[16], Node * pt[16], Node * ct[16], Node & m, Node & mp, Node * mt[4]) {

    Node * mpt[4]; // M' table : (M1', M2', M3', M4')

    // Init mpt
    mpt[0] = mt[0];
    mpt[1] = mt[1];
    mpt[2] = mt[2];
    mpt[3] = mt[3];
    mix_column(mpt);

    Node * x[4][4];
    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 4; j += 1) {
            x[i][j] = pt[i * 4 + j];
        }
    }

    // Masking plain text with (M1', M2', M3', M4')
    // - with M1' on the first row
    // - with M2' on the second row
    // - with M3' on the third row
    // - with M4' on the forth row
    mask_plain_text(x, mpt);


    /* ---------------------- START Analysis ---------------------------*/
    
    Node * round_key[11 * 16];
    std::cout << "# Start Key Schedule" << std::endl;
    masked_init_masked_aes_keys(key, round_key, m, mp, mpt);
    std::cout << "# End of Key Schedule, starting AES" << std::endl;

    // Rounds
    for (int rnd = 0; rnd < 9; rnd += 1) {
        std::cout << "# Round " << rnd << std::endl;
        std::cout << "# ARK" << std::endl;
        // Add round key: changes mask from (M1', M2', M3', M4') to M
        add_round_key(x, round_key, rnd);

        std::cout << "# SB" << std::endl;
        // SBox': changes mask M to M'
        masked_sub_bytes(x);
        
        std::cout << "# ShiftRows" << std::endl;
        // Shift rows: does not change mask
        shift_rows(x);

        std::cout << "# ChangeMasks" << std::endl;
        // Changes masks from M' to (M1, M2, M3, M4)
        change_masks(x, mp, mt);


        std::cout << "# MixColumns" << std::endl;
        // Mix columns: changes masks from  (M1, M2, M3, M4) to (M1', M2', M3', M4')
        Node * r[4];
        for (int col = 0; col < 4; col += 1) {
            r[0] = x[0][col];
            r[1] = x[1][col];
            r[2] = x[2][col];
            r[3] = x[3][col];
            mix_column(r);
            x[0][col] = r[0];
            x[1][col] = r[1];
            x[2][col] = r[2];
            x[3][col] = r[3];
        }

        std::cout << "# End of Round " << rnd << std::endl;
        std::cout << "# Nb. expressions analysed: " << nbCheck << std::endl;
        std::cout << "# Nb. expressions leaking: " << nbLeak << std::endl;
    }

    // Final Round (no Mix Columns)
    // Add round key: changes mask from (M1', M2', M3', M4') to M
    std::cout << "# Final Round (9)" << std::endl;
    std::cout << "# ARK" << std::endl;
    add_round_key(x, round_key, 9);
    std::cout << "# SB" << std::endl;
    masked_sub_bytes(x);
    std::cout << "# ShiftRows" << std::endl;
    shift_rows(x);

    /* ---------------------- End Analysis ---------------------------*/

    std::cout << "# ARK" << std::endl;
    add_round_key_no_verif(x, round_key, 10); // must leak because the masks are removed...

    // Writing output ciphered text
    for (int i = 0; i < 16; i += 1) {
        // ct[i] = x[i % 4][int(i / 4)]
        Node * e = x[i % 4][i / 4];
        //checkExpLeakage(e); // leaks because masks have been removed
        ct[i] = e;
    }
    std::cout << "# End of AES" << std::endl;
}


int main(int argc, const char ** argv) {

    Node * pt[16];
    Node * key[16];
    Node * mt[4];
    Node * m;
    Node * mp;

    if (use_litteral_values) {
        for (int i = 0; i < 16; i += 1) {
            pt[i] = &constant(0, 8);
        }
        key[0] = &constant(0x80, 8);
        for (int i = 1; i < 16; i += 1) {
            key[i] = &constant(0, 8);
        }

        m = &constant(0xae, 8);
        mp = &constant(0x5f, 8);
        mt[0] = &constant(0x62, 8);
        mt[1] = &constant(0x81, 8);
        mt[2] = &constant(0x0c, 8);
        mt[3] = &constant(0x9e, 8);

        std::cout << "# Plain text:    ";
        for (int i = 0; i < 16; i += 1) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << pt[i]->cst[0] << std::dec;
        }
        std::cout << std::endl;
        std::cout << "# Key:           ";
        for (int i = 0; i < 16; i += 1) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << key[i]->cst[0] << std::dec;
        }
        std::cout << std::endl;
    
        for (int i = 0; i < 256; i += 1) {
            sboxp[i] = sbox[i ^ m->cst[0]] ^ mp->cst[0];
        }
        registerArray("sbox", 8, 8, 0x0, 512, NULL, sbox, 1);
        registerArray("sboxp", 8, 8, 0x0, 256, sboxp_func, sboxp, 1);
    }
    else {
        for (int i = 0; i < 16; i += 1) {
            char buffer[16];
            std::sprintf(buffer, "pt%02d", i);
            std::string ptSymb = buffer;
            pt[i] = &symbol(ptSymb, 'P', 8);
        }

        for (int i = 0; i < 16; i += 1) {
            char buffer[16];
            std::sprintf(buffer, "k%02d", i);
            std::string keySymb = buffer;
            key[i] = &symbol(keySymb, 'S', 8);
        }

        m = &symbol("m", 'M', 8); // M
        mp = &symbol("mp", 'M', 8); // M'
        for (int i = 0; i < 4; i += 1) {
            char buffer[16];
            std::sprintf(buffer, "m%d", i);
            std::string mtSymb = buffer;
            mt[i] = &symbol(mtSymb, 'M', 8);
        }

        registerArray("sbox", 8, 8, 0x0, 512, NULL, sbox, 1);
        registerArray("sboxp", 8, 8, 0x0, 256, sboxp_func, sboxp, 1);
    }

    Node * ct[16];
    aes_herbst(key, pt, ct, *m, *mp, mt);

    if (use_litteral_values) {
        std::cout << "# Ciphertext : ";
        for (int i = 0; i < 16; i += 1) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) << ct[i]->cst[0] << std::dec;
        }
        std::cout << std::endl;
    }

    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    return 0;
}

