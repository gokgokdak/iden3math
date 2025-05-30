#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pyc/helper_py.h"
#include <iden3math/fp1.h>

namespace py = pybind11;

using namespace iden3math;

void init_fp1(py::module_& m) {
    py::class_<Fp1>(m, "Fp1")
    .def(py::init<const BigInt&>(), py::arg("prime"), "Initialize the prime field with the given prime number.")
    .def("mod_reduce", &Fp1::mod_reduce, py::arg("a"), "Reduce the given integer modulo the field's prime.")
    .def("add", &Fp1::add, py::arg("a"), py::arg("b"), "Add two integers in the field.")
    .def("sub", &Fp1::sub, py::arg("a"), py::arg("b"), "Subtract two integers in the field.")
    .def("mul", &Fp1::mul, py::arg("a"), py::arg("b"), "Multiply two integers in the field.")
    .def("div", &Fp1::div, py::arg("a"), py::arg("b"), "Divide two integers in the field. Returns None if division by zero or not 'b' does not have a modular inverse.")
    .def("pow", &Fp1::pow, py::arg("a"), py::arg("b"), "Raise an integer to a power in the field. Returns None if 'a' does not have a modular inverse.")
    .def("square", &Fp1::square, py::arg("a"), "Square an integer in the field.")
    .def("sqrt", &Fp1::sqrt, py::arg("a"), "Compute the square root of an integer in the field. Returns None if 'a' does not have a square root.")
    .def("mod_inv", &Fp1::mod_inv, py::arg("a"), "Compute the modular inverse of an integer in the field. Returns None if 'a' does not have a modular inverse.")
    .def("neg", &Fp1::neg, py::arg("a"), "Negate an integer in the field.")
    .def("has_sqrt", &Fp1::has_sqrt, py::arg("a"), "Check if an integer has a square root in the field.")
    ;
}

#endif
