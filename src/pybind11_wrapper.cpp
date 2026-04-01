
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <verif_msi_pp.hpp>
 
#include <map>


namespace py = pybind11;


Node & bindConcat(py::args & args) {
    std::vector<Node *> pyArgs;
    for (const auto & arg : args) {
        pyArgs.push_back(&py::cast<Node &>(arg));
    }
    return Concat(pyArgs);
}


Node & bindGMul(py::args & args) {
    std::vector<Node *> pyArgs;
    for (const auto & arg : args) {
        pyArgs.push_back(&py::cast<Node &>(arg));
    }
    return GMul(pyArgs);
}


py::tuple bindCompareExpsWithExev(Node & e0, Node & e1) {
    std::map<Node *, Node *> res;
    uint64_t v0;
    uint64_t v1;
    bool equal = compareExpsWithExev(e0, e1, res, &v0, &v1);
    if (equal) {
        return py::make_tuple(py::none(), py::none(), py::none());
    }
    else {
        py::dict pyres;
        for (const auto & [k, v] : res) {
            pyres[py::cast<Node &>(*k)] = py::cast<Node &>(*v);
        }
        return py::make_tuple(pyres, v0, v1);
    }
}


py::tuple bindCompareExpsWithRandev(Node & e0, Node & e1, int32_t nbEval) {
    std::map<Node *, Node *> res;
    uint64_t v0;
    uint64_t v1;
    bool equal = compareExpsWithRandev(e0, e1, nbEval, res, &v0, &v1);
    if (equal) {
        return py::make_tuple(py::none(), py::none(), py::none());
    }
    else {
        py::dict pyres;
        for (const auto & [k, v] : res) {
            pyres[py::cast<Node &>(*k)] = py::cast<Node &>(*v);
        }
        return py::make_tuple(pyres, v0, v1);
    }
}


py::tuple bindGetDistribWithExev(Node & e) {
    bool rud;
    bool retval = getDistribWithExev(e, &rud);
    return py::make_tuple(retval, rud);
}


void bindDumpCircuit(const char * filename, py::args & outputs) {
    std::vector<HWElement *> cppOutputs;
    for (const auto & output : outputs) {
        cppOutputs.push_back(&py::cast<HWElement &>(output));
    }
    dumpCircuit(filename, cppOutputs);
}


py::tuple bindCheckSecurity(int32_t order, bool withGlitches, const std::string & secProp, py::args & outputs) {
    SecurityProperty sp = PROP_NONE;
    int32_t nbCheck;
    if (secProp == "tps") {
        sp = TPS;
    }
    else if (secProp == "ni") {
        sp = NI;
    }
    else if (secProp == "sni") {
        sp = SNI;
    }
    else if (secProp == "rni") {
        sp = RNI;
    }
    else if (secProp == "pini") {
        sp = PINI;
    }
    else {
        std::cout << "*** Error: Unknown security property " << secProp << std::endl;
    }
    std::vector<HWElement *> cppOutputs;
    for (const auto & o : outputs) {
        cppOutputs.push_back(&py::cast<HWElement &>(o));
    }
    int32_t nbLeak;
    nbLeak = checkSecurity(order, withGlitches, sp, cppOutputs, &nbCheck);
    return py::make_tuple(nbLeak, nbCheck);
}


py::tuple bindGetRealShares(Node & s, int nbShares) {
    auto tup = py::tuple(nbShares);
    std::vector<Node *> shares = getRealShares(s, nbShares);
    for (int32_t i = 0; i < nbShares; i += 1) {
        tup[i] = shares[i];
    }
    return tup;
}


py::tuple bindGetPseudoShares(Node & s, int nbShares) {
    py::tuple tup(nbShares);
    std::vector<Node *> shares = getPseudoShares(s, nbShares);
    for (int32_t i = 0; i < nbShares; i += 1) {
        tup[i] = shares[i];
    }
    return tup;
}


py::object bindLitteralInteger(Node & e) {
    uint64_t val;
    bool isLitteral = litteralInteger(e, &val);
    if (isLitteral) {
        return py::cast(val);
    }
    else {
        return py::none();
    }
}


void bindRegisterArray(std::string & name, int32_t inWidth, int32_t outWidth, py::object addr, int32_t size, std::function<Node&(Node *, Node &)> & func, py::list & content) {

    int32_t elemSize = size / (int32_t) content.size();
    assert(size == elemSize * (int32_t) content.size());

    uint64_t cppAddr;
    if (addr.is(py::none())) {
        cppAddr = 0x0;
    }
    else {
        cppAddr = py::cast<uint64_t>(addr);
    }

    uint8_t * cppContent = (uint8_t *) malloc(sizeof(uint8_t) * content.size());
    int32_t i = 0;
    for (const auto & v : content) {
        if (elemSize == 1) {
            cppContent[i] = py::cast<uint8_t>(v);
        }
        else if (elemSize == 2) {
            ((uint16_t *) cppContent)[i] = py::cast<uint16_t>(v);
        }
        else if (elemSize == 4) {
            ((uint32_t *) cppContent)[i] = py::cast<uint32_t>(v);
        }
        else if (elemSize == 8) {
            ((uint64_t *) cppContent)[i] = py::cast<uint64_t>(v);
        }
        else {
            assert(false);
        }
        i += 1;
    }

    //if (py::cast<py::object>(func) == py::none()) {
    //    registerArray(name, inWidth, outWidth, NULL, size, func, cppContent, elemSize);
    //}
    //else {
        registerArray(name, inWidth, outWidth, cppAddr, size, func, cppContent, elemSize);
    //}
}


py::tuple bindGetArrayAndOffset(Node & addr) {
    Node * offset;
    ArrayExp & arr = getArrayAndOffset(addr, &offset);
    Node & off = *offset;
    return py::make_tuple(arr, off);
}


HWElement & bindAndGate(py::args & args) {
    std::vector<HWElement *> pyArgs;
    for (const auto & arg : args) {
        pyArgs.push_back(&py::cast<HWElement &>(arg));
    }
    return andGate(pyArgs);
}


HWElement & bindOrGate(py::args & args) {
    std::vector<HWElement *> pyArgs;
    for (const auto & arg : args) {
        pyArgs.push_back(&py::cast<HWElement &>(arg));
    }
    return orGate(pyArgs);
}


HWElement & bindXorGate(py::args & args) {
    std::vector<HWElement *> pyArgs;
    for (const auto & arg : args) {
        pyArgs.push_back(&py::cast<HWElement &>(arg));
    }
    return xorGate(pyArgs);
}


py::tuple bindCheckTpsVal(Node & e) {
    bool usedBitExp;
    uint64_t timeRet;
    bool res = checkTpsVal(e, &usedBitExp, &timeRet);
    py::tuple retval(3);
    retval[0] = res;
    retval[1] = usedBitExp;
    retval[2] = timeRet;
    return retval;
}


py::tuple bindCheckNIVal(Node & e, int maxShareOcc) {
    bool usedBitExp;
    uint64_t timeRet;
    bool res = checkNIVal(e, maxShareOcc, &usedBitExp, &timeRet);
    py::tuple retval(3);
    retval[0] = res;
    retval[1] = usedBitExp;
    retval[2] = timeRet;
    return retval;
}


py::tuple bindCheckRNIVal(Node & e, int diff) {
    bool usedBitExp;
    uint64_t timeRet;
    bool res = checkRNIVal(e, diff, &usedBitExp, &timeRet);
    py::tuple retval(3);
    retval[0] = res;
    retval[1] = usedBitExp;
    retval[2] = timeRet;
    return retval;
}


py::tuple bindCheckPINIVal(Node & e, int maxShareOcc, py::args & args) {
    bool usedBitExp;
    uint64_t timeRet;
    std::set<int> outputIndexes;
    for (const auto & arg : args) {
        outputIndexes.insert(py::cast<int>(arg));
    }
    bool res = checkPINIVal(e, maxShareOcc, outputIndexes, &usedBitExp, &timeRet);
    py::tuple retval(3);
    retval[0] = res;
    retval[1] = usedBitExp;
    retval[2] = timeRet;
    return retval;
}


py::tuple bindCheckTpsTrans(Node & e0, Node & e1) {
    bool usedBitExp;
    uint64_t timeRet;
    bool res = checkTpsTrans(e0, e1, &usedBitExp, &timeRet);
    py::tuple retval(3);
    retval[0] = res;
    retval[1] = usedBitExp;
    retval[2] = timeRet;
    return retval;
}


py::tuple bindCheckNITrans(Node & e0, Node & e1, int maxShareOcc) {
    bool usedBitExp;
    uint64_t timeRet;
    bool res = checkNITrans(e0, e1, maxShareOcc, &usedBitExp, &timeRet);
    py::tuple retval(3);
    retval[0] = res;
    retval[1] = usedBitExp;
    retval[2] = timeRet;
    return retval;
}


py::tuple bindCheckRNITrans(Node & e0, Node & e1, int diff) {
    bool usedBitExp;
    uint64_t timeRet;
    bool res = checkRNITrans(e0, e1, diff, &usedBitExp, &timeRet);
    py::tuple retval(3);
    retval[0] = res;
    retval[1] = usedBitExp;
    retval[2] = timeRet;
    return retval;
}


py::tuple bindCheckPINITrans(Node & e0, Node & e1, int maxShareOcc, py::args & args) {
    bool usedBitExp;
    uint64_t timeRet;
    std::set<int> outputIndexes;
    for (const auto & arg : args) {
        outputIndexes.insert(py::cast<int>(arg));
    }
    bool res = checkPINITrans(e0, e1, maxShareOcc, outputIndexes, &usedBitExp, &timeRet);
    py::tuple retval(3);
    retval[0] = res;
    retval[1] = usedBitExp;
    retval[2] = timeRet;
    return retval;
}


bool bindPini(Node & e, int maxShareOcc, py::args & args) {
    std::set<int> outputIndexes;
    for (const auto & arg : args) {
        outputIndexes.insert(py::cast<int>(arg));
    }
    return pini(e, maxShareOcc, outputIndexes, false);
}



PYBIND11_MODULE(verif_msi_pp, m) {
    m.doc() = "verif_msi_pp plugin";
    py::class_<Node>(m, "Node")
        .def("__xor__", &Node::operator^, py::return_value_policy::reference)
        .def("__and__", &Node::operator&, py::return_value_policy::reference)
        .def("__or__", &Node::operator|, py::return_value_policy::reference)
        .def("__invert__", &Node::operator~, py::return_value_policy::reference)
        .def("__add__", &Node::operator+, py::return_value_policy::reference)
        .def("__sub__", py::overload_cast<Node &>(&Node::operator-), py::return_value_policy::reference)
        .def("__neg__", py::overload_cast<>(&Node::operator-), py::return_value_policy::reference)
        .def("__lshift__", py::overload_cast<int>(&Node::operator<<), py::return_value_policy::reference)
        .def("__lshift__", py::overload_cast<Node &>(&Node::operator<<), py::return_value_policy::reference)
        .def("__rshift__", py::overload_cast<int>(&Node::operator>>), py::return_value_policy::reference)
        .def("__rshift__", py::overload_cast<Node &>(&Node::operator>>), py::return_value_policy::reference)
        .def("__mul__", &Node::operator*, py::return_value_policy::reference)
        .def("__str__", &Node::toString)
        .def("expPrint", &Node::expPrint);
    py::class_<HWElement>(m, "HWElement")
        .def("__str__", &HWElement::toString)
        .def("getSymbExp", &HWElement::getSymbExp, py::return_value_policy::reference);
    py::class_<ArrayExp>(m, "ArrayExp")
        .def("__getitem__", &ArrayExp::operator[], py::return_value_policy::reference);
        
    m.def("LShR", py::overload_cast<Node &, int>(&LShR), "Desc.", py::return_value_policy::reference);
    m.def("LShR", py::overload_cast<Node &, Node &>(&LShR), "Desc.", py::return_value_policy::reference);
    m.def("RotateRight", py::overload_cast<Node &, int>(&RotateRight), "Desc.", py::return_value_policy::reference);
    m.def("RotateRight", py::overload_cast<Node &, Node &>(&RotateRight), "Desc.", py::return_value_policy::reference);
    m.def("Concat", &bindConcat, "Desc.", py::return_value_policy::reference);
    m.def("Extract", py::overload_cast<int, int, Node &>(&Extract), "Desc.", py::return_value_policy::reference);
    m.def("Extract", py::overload_cast<Node &, Node &, Node &>(&Extract), "Desc.", py::return_value_policy::reference);
    m.def("ZeroExt", py::overload_cast<int, Node &>(&ZeroExt), "Desc.", py::return_value_policy::reference);
    m.def("ZeroExt", py::overload_cast<Node &, Node &>(&ZeroExt), "Desc.", py::return_value_policy::reference);
    m.def("SignExt", py::overload_cast<int, Node &>(&SignExt), "Desc.", py::return_value_policy::reference);
    m.def("SignExt", py::overload_cast<Node &, Node &>(&SignExt), "Desc.", py::return_value_policy::reference);
    m.def("GMul", &bindGMul, "Desc.", py::return_value_policy::reference);
    m.def("GPow", &GPow, "Desc.", py::return_value_policy::reference);
    m.def("GLog", &GLog, "Desc.", py::return_value_policy::reference);

    m.def("checkTpsVal", &bindCheckTpsVal, "Desc.");
    m.def("checkNIVal", &bindCheckNIVal, "Desc.");
    m.def("checkRNIVal", &bindCheckRNIVal, "Desc.");
    m.def("checkPINIVal", &bindCheckPINIVal, "Desc.");
    m.def("checkTpsTrans", &bindCheckTpsTrans, "Desc.");
    m.def("checkNITrans", &bindCheckNITrans, "Desc.");
    m.def("checkRNITrans", &bindCheckRNITrans, "Desc.");
    m.def("checkPINITrans", &bindCheckPINITrans, "Desc.");
    
    m.def("compareExpsWithExev", &bindCompareExpsWithExev, "Desc.");
    m.def("compareExpsWithRandev", &bindCompareExpsWithRandev, "Desc.");
    m.def("getDistribWithExev", &bindGetDistribWithExev, "Desc.");

    m.def("andGate", &bindAndGate, py::return_value_policy::reference);
    m.def("orGate", &bindOrGate, py::return_value_policy::reference);
    m.def("xorGate", &bindXorGate, py::return_value_policy::reference);
    m.def("notGate", &notGate, py::return_value_policy::reference);
    m.def("inputGate", &inputGate, py::return_value_policy::reference);
    m.def("Register", &Register, py::return_value_policy::reference);
    m.def("dumpCircuit", &bindDumpCircuit, "Desc.");
    m.def("checkSecurity", &bindCheckSecurity, "Desc.");

    m.def("getBitDecomposition", &getBitDecomposition, "Desc.", py::return_value_policy::reference);
    m.def("simplify", &simplify, "Desc.", py::return_value_policy::reference);
    m.def("simplifyAndNotPEI", &simplifyAndNotPEI, "Desc.", py::return_value_policy::reference);
    m.def("simplifyCore", &simplifyCore, "Desc.", py::return_value_policy::reference);
    m.def("equivalence", &equivalence, "Desc.");

    m.def("tps", py::overload_cast<Node &, bool, bool>(&tps), "Desc.", py::arg("nodeIn"), py::arg("bitDecompose") = false, py::arg("verbose") = false);
    m.def("ni", py::overload_cast<Node &, int, bool, bool>(&ni), "Desc.", py::arg("nodeIn"), py::arg("maxShareOcc"), py::arg("bitDecompose") = false, py::arg("verbose") = false);
    m.def("rni", py::overload_cast<Node &, int, bool, bool>(&rni), "Desc.", py::arg("nodeIn"), py::arg("diff"), py::arg("bitDecompose") = false, py::arg("verbose") = false);
    m.def("pini", &bindPini, "Desc.");

    m.def("symbol", &symbol, "Desc.", py::return_value_policy::reference);
    m.def("constant", &constant, "Desc.", py::return_value_policy::reference);
    m.def("litteralInteger", &bindLitteralInteger, "Desc.");
    m.def("registerArray", &bindRegisterArray, "Desc.");
    m.def("getArrayByAddr", &getArrayByAddr, "Desc.", py::return_value_policy::reference);
    m.def("getArrayByName", &getArrayByName, "Desc.", py::return_value_policy::reference);
    m.def("getArrayFuncByAddr", &getArrayFuncByAddr);
    m.def("getArrayFuncByName", &getArrayFuncByName);
    m.def("getArrayAndOffset", &bindGetArrayAndOffset);

    m.def("checkResults", py::overload_cast<Node &, Node &>(&checkResults), "Desc.");
    m.def("checkResults", py::overload_cast<Node &, Node &, bool, bool>(&checkResults), "Desc.");

    m.def("checkTpsResult", &checkTpsResult, "Desc.");
    m.def("checkNIResult", &checkNIResult, "Desc.");
    m.def("getPseudoShares", &bindGetPseudoShares, "Desc.");
    m.def("getRealShares", &bindGetRealShares, "Desc.");

}


