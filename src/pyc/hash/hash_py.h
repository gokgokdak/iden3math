#pragma once

#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_hash_blake(py::module_& m);
void init_hash_keccak(py::module_& m);
void init_hash_mimc(py::module_& m);
void init_hash_pedersen(py::module_& m);

inline void init_hash(py::module_& m) {
    auto hash = m.def_submodule("hash");
    init_hash_blake(hash);
    init_hash_keccak(hash);
    init_hash_mimc(hash);
    init_hash_pedersen(hash);
}

#endif
