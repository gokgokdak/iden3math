#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include "pyc/helper_py.h"
#include <iden3math/hash/keccak.h>

namespace py = pybind11;

using namespace iden3math;

void init_hash_keccak(py::module_& m) {
    m.def("keccak256", [](const ByteVec1D& bytes) -> ByteVec1D {
            ByteVec1D digest;
            hash::keccak256(bytes, digest);
            return digest;
        },
        py::arg("bytes"),
        "Compute Keccak256 hash of a byte array."
    )
    .def("keccak256", [](const std::string& text) -> ByteVec1D {
            ByteVec1D digest;
            hash::keccak256(text, digest);
            return digest;
        },
        py::arg("text"),
        "Compute Keccak256 hash of a string."
    )
    .def("keccak256", [](const BigInt& number) -> ByteVec1D {
            ByteVec1D digest;
            hash::keccak256(number, digest);
            return digest;
        },
        py::arg("number"),
        "Compute Keccak256 hash of a number."
    )
    ;
}

#endif
