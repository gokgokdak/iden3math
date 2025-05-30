#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include <iden3math/typedef.h>

namespace py = pybind11;

using namespace iden3math;

void init_typedef(py::module_& m) {
    m.attr("Bit")  = py::bool_(false);
    m.attr("Byte") = py::int_(0);
    py::enum_<Endian>(m, "Endian")
        .value("LE", LE, "Little Endian")
        .value("BE", BE, "Big Endian")
        .export_values();
}

#endif
