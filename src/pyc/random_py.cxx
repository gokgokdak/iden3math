#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include "pyc/helper_py.h"
#include <iden3math/random.h>

namespace py = pybind11;

using namespace iden3math;

void init_random(py::module_& m) {
    m.def_submodule("random")
    .def("get_bytes", [](size_t len) -> ByteVec1D {
            return random::get_bytes(len, std::nullopt);
        },
        py::arg("len"),
        "Generate a random byte vector of the specified length."
    )
    .def("get_integer", [](const BigInt& p) -> BigInt {
            return random::get_integer(p, std::nullopt);
        },
        py::arg("p"),
        "Generate a random integer in the range [1, p)."
    )
    ;
}

#endif
