
# VerifMSI++: Practical Verification of Masking Schemes Implementations

VerifMSI++ is a formal verification tool for the probing security of masked implementations at different abstraction levels. It provides a way for verifying different security properties on a set of symbolic expressions or on stateless hardware circuits (gadgets).

VerifMSI++ is a C++ rewritten version of [VerifMSI](https://github.com/quentin-meunier/VerifMSI) with more simplification rules, additional functionalities and some bug fixes. It offers a set of constructs and functions for writing and verifying symbolic expressions, at algorithmic or gate level. A symbolic expression in VerifMSI++ is a fixed width expression comprising operations on constants and symbolic variables. Each symbolic variable has a type between secret, share, mask and public. VerifMSI++ verifies that the value distribution of a set of symbolic expressions value respects the constraints associated to the security property, considering that mask variables follow an independent random uniform distribution.

Since this work is not associated to any publication, should it be cited, the original VerifMSI article should be used:
Q. L. Meunier and A. Taleb (2023). VerifMSI: Practical Verification of Hardware and Software Masking Schemes Implementations. In Proceedings of the 20th International Conference on Security and Cryptography, ISBN 978-989-758-666-8, ISSN 2184-7711, pages 520-527. DOI: [10.5220/0012138600003555](https://dx.doi.org/10.5220/0012138600003555)
(Full paper version: [https://eprint.iacr.org/2023/732](https://eprint.iacr.org/2023/732))

VerifMSI++ is used in the [aLEAKator project](https://github.com/noeamiot/aLEAKator) [19] -- [article](https://eprint.iacr.org/2025/2193)

Currently, the C++ to python binding allowing to call the C++ implementation from a python program is broken, but it should be back in April 2026.



## Installation

VerifMSI++ has only been tested on Linux so far. In order to build it, only a C++ compiler with standard libraries is required: simply type `make` in the main directory. It will create two versions of a the library: a static and a dynamic one.


## Usage

In order to use VerifMSI constructs, the file `verif_msi_pp.hpp' from the `include` directory must be included.

Symbolic variables are created with a function called `symbol()`: the first parameter is the symbol name, the second parameter the symbol type ('S' for secret, 'M' for mask and 'P' for a public variable), and the third parameter the symbol width. Constants values are created with a function called `constant()`, taking as parameters the value and the width in bits.

Example:
```
#include "verif_msi_pp.hpp"

int main() {
    // Creating an 8-bit secret variable named "k"
    Node & k = symbol("k", 'S', 8);

    // Creating an 8-bit mask variable named "m0"
    Node & m = symbol("m", 'M', 8);

    // Creating the 8-bit constant 0xAE
    Node & c = constant(0xAE, 8);

    // Computing an expression
    Node & e = k ^ m ^ c;

    // Checking Threshold Probing Security on e
    bool res = checkTpsVal(e);

    if (res) {
        std::cout << "# Expression " << e << " is probing secure" << std::endl;
    }
    else {
        std::cout << "# Expression " << e << " is not probing secure" << std::endl;
    }

    return 0;
}
```

> Note: by the way nodes are declared using aliases, and the way C++ works, it is not possible to reaffect a declared variable, as it was with VerifMSI. For example, the following code shall not be written:
> `Node & n0 = e;`
> `Node & n = k ^ m;`
> `n = n & n0; // incorrect`

VerifMSI++ also supports hardware constructs (gates and registers) for verifying gadgets implementations. The following code implements the Domain Oriented Masking AND from [1] with two shares. The function `getRealShares()` allows to split a secret into a specified number of shares. In this case, the returned elements are symbols typed as shares, such that the linear recombination (xor) of all the shares is the secret. An alternate function, `getPseudoShares()`, allows to make an explicit sharing based on the secret and dedicated masks. The difference in the secret representation (explicit or shares) determines which security properties can be verified.

```
#include "verif_msi_pp.hpp"

int32_t order = 1;
bool withGlitches = true;
bool noFalsePositive = false;

int main() {
    Node & a = symbol("a", 'S', 1);
    Node & b = symbol("b", 'S', 1);
    Node & n_z1_0 = symbol("z1_0", 'M', 1);

    // Do the sharing for 'a' and 'b'
    std::vector<Node *> v_a = getRealShares(a, 2);
    std::vector<Node *> v_b = getRealShares(b, 2);

    // Get shares as individual nodes
    Node & n_a0 = *v_a[0];
    Node & n_a1 = *v_a[1];
    Node & n_b0 = *v_b[0];
    Node & n_b1 = *v_b[1];

    // Create input gates
    HWElement & a0 = inputGate(n_a0);
    HWElement & a1 = inputGate(n_a1);
    HWElement & b0 = inputGate(n_b0);
    HWElement & b1 = inputGate(n_b1);
    HWElement & z1_0 = inputGate(n_z1_0);

    // Cross products
    HWElement & a0b0i = andGate(a0, b0);
    HWElement & a0b1i = andGate(a0, b1);
    HWElement & a1b0i = andGate(a1, b0);
    HWElement & a1b1i = andGate(a1, b1);

    // Remaining gates and registers
    HWElement & a1b0x = xorGate(a1b0i, z1_0);
    HWElement & a1b0r = Register(a1b0x);
    HWElement & a0b1x = xorGate(a0b1i, z1_0);
    HWElement & a0b1r = Register(a0b1x);
    HWElement & c0_0 = a0b0i;
    HWElement & c0 = xorGate(c0_0, a0b1r);
    HWElement & c1_0 = a1b1i;
    HWElement & c1 = xorGate(c1_0, a1b0r);

    std::vector<HWElement *> outputs;
    outputs.push_back(&c0);
    outputs.push_back(&c1);

    // Check the NI security property
    int32_t nbCheck;
    int32_t nbLeak = checkSecurity(order, withGlitches, NI, outputs, noFalsePositive, &nbCheck);

    std::cout << "# Total Nb. of expressions analysed: " << nbCheck << std::endl;
    std::cout << "# Total Nb. of potential leakages found: " << nbLeak << std::endl;

    return 0;
}
```

The gadget inputs must be defined with the `inputGate()` function; the available logic gates are `andGate`, `orGate`, `xorGate` and `notGate`. The `Register`construct allows to stop the propagation of glitches. Note that the register construct does not allow to design stateful gadgets, i.e. gadgets requiring a simulation to compute at each clock cycle the symbolic expression corresponding to each wire. This implies that gadgets cannot contain combinatorial or sequential loops.

The function `checkSecurity()` takes as parameters the security order, a boolean indicating if glitches must be considered, the security property (see corresponding section), a list of wires corresponding to the outputs of the gadget, and a boolean indicating if enumeration should be performed in case of a potential secret leakage (see corresponding section).


## Security Properties

VerifMSI supports the verification of 6 security properties:
* Threshold Probing Security (TPS) [2], requiring an explicit secret representation;
* Non-Interference (NI) [3], requiring a share representation;
* Strong Non-Interference (SNI) [3], requiring a share representation;
* Probe Isolating Non-Interference (PINI) [4], requiring a share representation;
* Output-PINI (OPINI) [15], requiring a share representation;
* Relaxed Non-Interference (RNI), requiring a share representation and defined in VerifMSI article.
NI, SNI, RNI, PINI and OPINI are only defined for gadgets, as they make assumptions on the set of all probes to verify (e.g. probes corresponding to single shares are always accessible)
[FIXME] / [TODO]: être plus précis / donner des exemples, que se passe-t-il si on appelle NI sur un tuple ?


## Supported operations

* `^`: bitwise exclusive OR
* `&`: bitwise AND
* `|`: bitwise OR
* `~`: bitwise NOT
* `+`: arithmetic addition
* `-`: arithmetic subtraction
* `<<`: logical shift left. The shift amount must be a constant or a python integer, and cannot be symbolic.
* `>>`: arithmetic shift right. The shift amount must be a constant or a python integer, and cannot be symbolic.
* `*`: integer multiplication, modulo 2 to the power of the expression width.
* `**`: integer power, modulo 2 to the power of the expression width.


Some operations are implemented in the form of functions:

* `LShR(x, y)`: logical shift right. The shift amount y must be a constant or a python integer, and cannot be symbolic.
* `RotateRight(x, y)`: right shift with rotation
* `Concat(x, y, ...)`: concatenation of expressions
* `Extract(msb, lsb, e)`: extraction of some of the bits in `e`, from the most significant bit given by msb, to the least significant bit given by lsb.
* `ZeroExt(v, e)`: zero extension: extension of the expression `e` by the addition of v bits with value 0 on the left of `e`
* `SignExt(v, e)`: signed extension: extension of the expression `e` by adding v time the MSB of `e` on its left

Some operations related to Galois Fields are implemented but haven't been much tested:
* `GMul(x, y, ...)`: Finite Field multiplication. Currently, it is only implemented on 8 bits, and with the irreducible polynomial 0x11B.
* `GPow(x, y)`: Finite Field power
* `GLog(x)`: Finite Field logarithm
* `GExp(x)`: Finite Field exponentiation


## Functionalities

### Simplification

VerifMSI implements a wide range of simplifications, taking advantage of operators properties. In order to simplify an expression, simply call the `simplify()` function:
```
Node & p0 = symbol("p0", 'P', 8);
Node & p1 = symbol("p1", 'P', 8);
Node & m = symbol("m", 'M', 8);
Node & e = ((p0 ^ m) | (p1 & constant(0, 8))) ^ (m & constant(0xFF, 8) + (p0 ^ p0));
Node & simplifiedExp = simplify(e);
std::cout << "simplifiedExp: " << simplifiedExp << std::endl;
```

### Manual Simplification Rules

Note: this functionality is temporarily broken and should not be used for now.


### Bit Decomposition of Expressions

VerifMSI++ can decompose an expression into a concatenation of 1-bit expressions. Although this feature is mostly used in the verification algorithm, it is possible to get the equivalent bit decomposition with the function `getBitDecomposition()`. n-bit symbolic variables are decomposed into n 1-bit variables of the same type.
```
Node & k = symbol("k", 'S', 4);
Node & exp = k & constant(0x5, 4);
Node & bitExp = getBitDecomposition(exp);
std::cout << "bitExp: " << bitExp << std::endl;
```


### Arrays

Symbolic array accesses can be modeled in VerifMSI++ expressions. Currently, only some types of arrays are considered, more specifically those in which the corresponding initialization in the non-symbolic implementation is made without using symbolic variables. This means that accessing to an array with expression `e` as index cannot leak more information than `e`, and prevents for example to have an index `i` such that `array[i] = exp`, where `exp` is a symbolic expression. This is particularly useful for algorithms like the AES in which the SBox can be implemented as an array. In order to create an array, one has to use the `registerArray()` function. Parameters are in order:
* Array name
* Input width (in bits)
* Output width (in bits)
* Array base address: useful for running low-level software implementations, for which adresses can be emitted in the form of the sum of an array base address and a symoblic offset. In that case, the function `getArrayAndOffset()` can be used to retrieve the array accessed along with the symbolic offset from an expression corresponding to the memory address accessed.
* Array size (in bytes): can be used to detect out of bounds accesses, unused in provided functions
* Array function: if the array implements a function of the index accessed, this can be used to transform an array access to the corresponding expression.
* Array concrete content: concrete values for the array, for concrete evalution
* Element size (1, 2, 4 or 8, in bytes): represents the size of elements in the concrete array, in order to access them correctly for concrete evaluation

The minimal required arguments are the name, input width and output width.

It is advised to specify the content of the array upon registration whenever possible, as this allows to evaluate the expression for concrete input values. This is very useful for checking an implementation, as it is sufficient to replace symbolic variables with constants in order to check the correctness of an implementation. This also allows to use the **concrete evaluation** module functions.

For example, the SBox from the AES can be implemented as follows:
```
#include "verif_msi_pp.hpp"

uint8_t sbox[256] = { 0x63, 0x7C, ...};

int main() {
    registerArray("sbox", 8, 8, 0x0, 256, NULL, sbox, 1);
    ArrayExp & sb = getArrayByName("sbox");

    Node & m = symbol("m", 'M', 8);

    std::cout << "sbox[m]: " << sb[m] << std::endl; // displays "sbox[m]: sbox[m]"
    std::cout << "sbox[0]: " << sb[constant(0, 8)] << std::endl; // displays "sbox[0]: 0x63"

    return 0;
}
```

Arrays can also be associated to a function, which is used as a substitute for the expression to use. This allows for example to implement multiplication by 2 or 3 in the AES with an array:
```
#include "verif_msi_pp.hpp"

Node & mul_02_func(Node & idx) {
    return GMul(constant(2, 8), idx);
}

int main() {
    registerArray("mul_02", 8, 8, 0x0, 256, mul_02_func, NULL, 0);
    ArrayExp & mul_02 = getArrayByName("mul_02");

    Node & m = symbol("m", 'M', 8);

    std::cout << "mul_02[m]: " << mul_02[m] << std::endl; // displays "mul_02[m]: GMul(m, 0x02)"
    std::cout << "mul_02[1]: " << mul_02[constant(1, 8)] << std::endl; // displays "mul_02[1]: 0x02"

    return 0;
}
```

It is also possible to use this functionality to implement a masked sbox access, taking advantage of the fact that we know the expression corresponding to an index. For example, in the Herbst scheme, SBox'[x ^ m] = SBox[x] ^ m' (here using global variables here to pass the masks `m` and `mp`)

```
#include "verif_msi_pp.hpp"

Node * global_m;
Node * global_mp;

Node & sboxp_func(Node & idx) {
    ArrayExp & sbox = getArrayByName("sbox");
    Node & m = *global_m;
    Node & mp = *global_mp;
    return simplify(sbox[idx ^ m] ^ mp);
}

int main() {
    registerArray("sbox", 8, 8, 0x0, 256, NULL, NULL, 1);
    registerArray("sboxp", 8, 8, 0x0, 256, sboxp_func, NULL, 1);
    ArrayExp & sboxp = getArrayByName("sboxp");

    Node & k = symbol("k", 'S', 8);
    Node & pt = symbol("pt", 'P', 8);
    Node & m = symbol("m", 'M', 8);
    Node & mp = symbol("mp", 'M', 8);

    global_m = &m;
    global_mp = &mp;

    Node & e = sboxp[k ^ pt ^ m];

    std::cout << "sboxp[k ^ pt ^ m]: " << e << std::endl; // displays "sboxp[k ^ pt ^ m]: sbox[k ^ pt] ^ mp"

    return 0;
}
```

Examples using a function can be found in the `aes_sm` benchmark.

Finally, arrays can also be associated to a base address and a size. The base address represents its memory implantation and the size the size it occupies in memory. This mechanism is useful when verifying assembly code. Combined with a function, if a memory access to the array (detected with the base address and size) has a symbolic part, this part can be used as the parameter of the function called. This allows for example to transform the expression Sbox'[x ^ m] into Sbox[x] ^ m' in a compiled version of the AES Herbst scheme.


### Concrete Evaluation

The concrev module provides functions based on concrete evaluation. This comes in two flavours: exhaustive evaluation and random evaluation. Exhaustive evaluation enumerates all possible symbolic variables' values, and random evaluation evaluate the expression for randomly chosen values. The provided functions are:

* `compareExpsWithExev()`: checks that two expressions `e0` and `e1` are equivalent for all possible combinations of inputs
* `compareExpsWithRandev()`: checks that two expressions `e0` and `e1` provide the same result for a specified number of random evaluations, for which the value of all symbolic variables is chosen randomly and uniformly. This can be useful when enumeration is not possible.
> Note: These two functions also have a variant with additional parameters in order to get each variable value for the first encountered case of non-equivalence.
* `getDistribWithExev()`: computes the distribution of an expression regarding its secret variables. The returned value is `true` iff the expression is independent from the secret variables' values; the optional paramter is set to `true` iff the distribution is uniform.
* `isTPSWithExev()`, `isNIWithExev()`, `isPINIWithExev()`, `isOPINIWithExev()`: these functions use exhaustive evaluation to verify respectively the TPS, NI, PINI and OPINI properties on a tuple of expressions.
* `getExpValue()`: allows to get the concrete value of a given expression, for a specified concrete mapping of all the symbolic variables it contains.
* `getSymbolicBitsNum()`: returns the total number of symbolic bits in an expression (may be useful to decide if enumerate or not).
* `getMaxSymbVarWidth()`: returns the maximum width among all symbolic variables width in an expression.


### Verification

For gadgets and hardware circuits, the main verification function is `checkSecurity(order, withGlitches, secProp, *outputs, noFalsePositive)`, with the following parameters:
* `order`: security order. For a security order equal to `n`, all tuples of size `n` must verify the security property;
* `withGlitches`: boolean indicating if glitches are propagated through gates;
* `secProp`: security property to verify, must be one on 'TPS', 'NI', 'SNI', 'RNI', 'PINI', 'OPINI';
* `outputs`: vector of wires/gate outputs (`HWElement *`) corresponding to the outputs of the gadget. If this parameter has the type `vector<HWElement *>`, then the gadget is assumed to contain a single output, and the elements of the vector are its ordered shares. If the gadget contains several outputs, then a `vector<vector<HWElement *>>` can be passed as parameter, the first dimension being the different outputs and the second the shares.

At a lower level, i.e. for verifying a tuple of expressions, the following function are defined:
* `tps`, `ni`, `rni`, `pini`, `opini` which verify the corresponding properties, either on a single node, or on a `vector<Node *>`. Note that there is no `sni` function, as it is implemented as making a `ni` verification with a potentially different number of maximum occurrences per share. All of these functions come with a `NoFalsePositive` version, which makes an exhaustive enumeration in case of detected leakage. It is actually useful to perform the exhaustive enumeration just when the verification fails (and not after the verification returns), since the expression which has undergone replacements in the verification process is likely to contain much less symbolic variables due to the replacements which have already occurred.

Finally, functions in the `check_leakage` module (e.g. `checkTPSVal()`) are wrappers for these functions, with heuristics to determine if a bit decomposition should be made, for non bit-decomposed expressions.


## Configuration options

VerifMSI++ has several configurations options [TODO].


## Python binding

VerifMSI++ can be compiled as a python library, to enable using the C++ implementation from a python VerifMSI program. In order to do so, it is required to change the `from verif_msi import *` in the VerifMSI file to `from verif_msi_pp import *`. In order to enable the compilation of the python library, the flag `BUILD_PYTHON` in the Makefile must be set.

> Note: This feature is currently broken but will be fixed soon.


## Benchmarks

The `benchmarks/hw` directory contains hardware gadgets from the literature implemented in VerifMSI++.

These are the following:
* ISW AND: The logical AND masking scheme from [2]
* ISW AND refresh: A combination of the ISW AND with a circular refresh on one of the input from [5]
* DOM AND: The Domain Oriented Masking implementation of the AND gate from [1], resistant to glitches;
* GMS AND: Two implementations of the AND gate using the Generalized Masking Scheme, described in the article, using respectively 3 and 5 shares from [9];
* PINI Mult: The PINI multiplication scheme from [8];
* OPINI1: The O-PINI1 gadget from [15]
* OPINI2: The O-PINI2 gadget from [15]
* HPC3: the HPC3 gadget from [16]
* HPC4: the HPC4 gadget from [17]
* TSM: the TSM gadget from [18]
* TSM+: the TSM+ gadget from [18], for 2 and 3 inputs
* OTSM: the OTSM gadget from [18]
For schemes which are defined for any order, the benchmark file is a generator which creates the benchmark file for the specified order.


## License

[GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html)



## References

[1] H. Groß, S. Mangard, & T. Korak. (2017). An efficient side-channel protected AES implementation with arbitrary protection order. In Topics in Cryptology–CT-RSA 2017: The Cryptographers' Track at the RSA Conference 2017, Springer International Publishing.

[2] Y. Ishai, A. Sahai, & D. Wagner. (2003). Private circuits: Securing hardware against probing attacks. In Annual International Cryptology Conference (pp. 463-481). Springer, Berlin, Heidelberg.

[3] G. Barthe, S. Belaïd, G. Cassiers, P. A. Fouque, B. Grégoire & F. X. Standaert. (2019). maskverif: Automated verification of higher-order masking in presence of physical defaults. In ESORICS 2019: 24th European Symposium on Research in Computer Security (pp. 300-318). Springer International Publishing.

[4] G. Cassiers & F. X. Standaert (2020). Trivially and efficiently composing masked gadgets with probe isolating non-interference. IEEE Transactions on Information Forensics and Security, 15, 2542-2555.

[5] T. De Cnudde, O. Reparaz, B. Bilgin, S. Nikova, V. Nikov & V. Rijmen. (2016). Masking AES with shares in hardware. In Cryptographic Hardware and Embedded Systems (CHES) 2016. Springer Berlin Heidelberg.

[6] A. Battistello, J. S. Coron,  E. Prouff & R. Zeitoun. (2016). Horizontal side-channel attacks and countermeasures on the ISW masking scheme. In Cryptographic Hardware and Embedded Systems (CHES 2016). Springer Berlin Heidelberg.

[7] N. Bordes & P. Karpman. (2021). Fast verification of masking schemes in characteristic two. 40th Annual International Conference on the Theory and Applications of Cryptographic Techniques, 2021. Springer International Publishing.

[8] W. Wang, F. Ji, J. Zhang & Y. Yu. (2023). Efficient Private Circuits with Precomputation. IACR Transactions on Cryptographic Hardware and Embedded Systems.

[9] O. Reparaz, B. Bilgin, S. Nikova, B. Gierlichs & I. Verbauwhede. (2015). Consolidating masking schemes. 35th Annual Cryptology Conference. Springer Berlin Heidelberg.

[10] S. Nikova, C. Rechberger & V. Rijmen. (2006). Threshold implementations against side-channel attacks and glitches. In ICICS (Vol. 4307, pp. 529-545).

[11] Q. L. Meunier, I. Ben El Ouahma & K. Heydemann. (2020). SELA: a Symbolic Expression Leakage Analyzer. In International Workshop on Security Proofs for Embedded Systems.

[12] C. Herbst, E. Oswald & S. Mangard. (2006). An AES smart card implementation resistant to power analysis attacks. In International conference on applied cryptography and network security (pp. 239-252). Springer, Berlin, Heidelberg.

[13] Y. Yao, M. Yang, C. Patrick, B. Yuce & P. Schaumont. (2018). Fault-assisted side-channel analysis of masked implementations. In 2018 IEEE International Symposium on Hardware Oriented Security and Trust (HOST) (pp. 57-64). IEEE.

[14] M. Rivain, and E. Prouff. (2010). Provably secure higher-order masking of AES. In International Workshop on Cryptographic Hardware and Embedded Systems (pp. 413-427). Springer, Berlin, Heidelberg.

[15] G. Cassiers and F. X. Standaert. (2021). Provably Secure Hardware Masking in the Transition- and Glitch-Robust Probing Model: Better Safe than Sorry. IACR Trans. Cryptogr. Hardw. Embed. Syst. 2021, 2 (2021), 136–158. https://tches.iacr.org/index.php/TCHES/article/view/8790/8390

[16] D. Knichel & A. Moradi (2022). Low-latency hardware private circuits. Proceedings of the 2022 ACM SIGSAC Conference on Computer and Communications Security. https://eprint.iacr.org/2022/507

[17] G. Cassiers, F.-X. Standaert & C. Verhamme (2024). Low-Latency Masked Gadgets Robust against Physical Defaults with Application to Ascon. IACR Transactions on Cryptographic Hardware and Embedded Systems, 2024(3), 603-633.

[18] H. Rahimi & A. Moradi (2026). TSM+ and OTSM-Correct Application of Time Sharing Masking in Round-Based Designs. Cryptology ePrint Archive. https://eprint.iacr.org/2026/004

[19] N. Amiot, Q. Meunier, K. Heydemann & E. Encrenaz. (2025). aLEAKator: HDL Mixed-Domain Simulation for Masked Hardware & Software Formal Verification. Cryptology ePrint Archive. 

