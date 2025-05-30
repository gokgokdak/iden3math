#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include "pyc/helper_py.h"
#include <iden3math/hash/pedersen.h>

namespace py = pybind11;

using namespace iden3math;

void init_hash_pedersen(py::module_& m) {
    m.def("pedersen", [](const ByteVec1D& preimage) -> ByteVec1D {
            ByteVec1D digest;
            hash::pedersen(preimage, digest);
            return digest;
        },
        py::arg("preimage"),
        "Compute the Pedersen hash of the given preimage."
    );
}

#endif
