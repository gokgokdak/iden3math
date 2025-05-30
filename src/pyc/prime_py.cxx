#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include "pyc/helper_py.h"
#include <iden3math/prime.h>

namespace py = pybind11;

using namespace iden3math;

void init_prime(py::module_& m) {
    m.def_submodule("prime")
    .def("bn254", &prime::bn254, "Get the BN254 prime number.")
    ;
}

#endif
