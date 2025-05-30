#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pyc/helper_py.h"
#include <iden3math/hash/mimc.h>

namespace py = pybind11;

using namespace iden3math;

void init_hash_mimc(py::module_& m) {
    m.def("mimc_sponge", [](const ByteVec2D& preimages, size_t outputs, const ByteVec1D& key,
                                    Endian preimage_endian, Endian key_endian, Endian digest_endian) -> ByteVec2D {
            ByteVec2D digests;
            hash::mimc_sponge(preimages, outputs, key, digests, preimage_endian, key_endian, digest_endian);
            return digests;
        },
        py::arg("preimages"), py::arg("outputs"), py::arg("key"),
        py::arg("preimage_endian") = BE, py::arg("key_endian") = BE, py::arg("digest_endian") = BE,
        "Compute MiMC sponge hash for multiple preimages with an optional key."
    )
    ;
}

#endif
