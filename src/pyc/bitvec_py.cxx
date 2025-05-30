#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include <iden3math/bitvec.h>

namespace py = pybind11;

using namespace iden3math;

void init_bitvec(py::module_& m) {
    py::class_<BitVec1D>(m, "BitVec1D")
    .def(py::init<>())
    .def("size", &BitVec1D::size, "Get the number of bits in the vector.")
    .def("push", &BitVec1D::push, py::arg("bit"), "Add a bit to the end of the vector.")
    .def("__getitem__", [](const BitVec1D& self, size_t index) -> Bit {
            return self[index];
        },
        py::arg("index"),
        "Get the bit at the specified index."
    )
    .def("__len__", &BitVec1D::size, "Get the number of bits in the vector.")
    ;
}

#endif
