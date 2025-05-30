#pragma once

#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_ec_point(py::module_& m);
void init_ec_babyjub(py::module_& m);

inline void init_ec(py::module_& m) {
    auto ec = m.def_submodule("ec");
    init_ec_point(ec);
    init_ec_babyjub(ec);
}

#endif
