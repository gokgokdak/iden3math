#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include "pyc/helper_py.h"
#include <iden3math/hash/blake.h>

namespace py = pybind11;

using namespace iden3math;

void init_hash_blake(py::module_& m) {
    m.def("blake256", [](const ByteVec1D& bytes) -> ByteVec1D {
            return hash::blake256(bytes);
        },
        py::arg("bytes"),
        "Compute BLAKE256 hash of a byte array."
    )
    .def("blake256", [](const std::string& text) -> ByteVec1D {
            return hash::blake256(text);
        },
        py::arg("text"),
        "Compute BLAKE256 hash of a text string."
    )
    ;
}

#endif
