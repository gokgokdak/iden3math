#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pyc/helper_py.h"
#include <iden3math/ec/twisted_edwards.h>

namespace py = pybind11;

using namespace iden3math;

void init_ec_twisted_edwards(py::module_& m) {
    auto ted = m.def_submodule("twisted_edwards");

    py::class_<ec::twisted_edwards::Curve>(ted, "Curve")
    .def(
        py::init<const Fp1&, BigInt, BigInt>(),
        py::arg("field"),
        py::arg("a"),
        py::arg("d"),
        py::keep_alive<1, 2>(),
        "Creates a twisted Edwards curve over the given field with parameters a and d."
    )
    .def("field", [](const ec::twisted_edwards::Curve& self) { return self.field(); }, "Returns the underlying finite field.")
    .def("a", &ec::twisted_edwards::Curve::a, "Returns the curve parameter a.")
    .def("d", &ec::twisted_edwards::Curve::d, "Returns the curve parameter d.")
    ;

    py::class_<ec::twisted_edwards::ExtPoint>(ted, "ExtPoint")
    .def(
        py::init<BigInt, BigInt, BigInt, BigInt>(),
        py::arg("X") = 0,
        py::arg("Y") = 1,
        py::arg("Z") = 1,
        py::arg("T") = 0,
        "Creates an extended twisted Edwards point with coordinates (X, Y, Z, T)."
    )
    .def("X", [](const ec::twisted_edwards::ExtPoint& self) { return self.X; }, "Returns the X coordinate.")
    .def("Y", [](const ec::twisted_edwards::ExtPoint& self) { return self.Y; }, "Returns the Y coordinate.")
    .def("Z", [](const ec::twisted_edwards::ExtPoint& self) { return self.Z; }, "Returns the Z coordinate.")
    .def("T", [](const ec::twisted_edwards::ExtPoint& self) { return self.T; }, "Returns the T coordinate.")
    .def("__eq__", &ec::twisted_edwards::ExtPoint::operator==, py::arg("other"), "Checks if two extended points are equal coordinate-wise.")
    .def("__ne__", &ec::twisted_edwards::ExtPoint::operator!=, py::arg("other"), "Checks if two extended points are not equal coordinate-wise.")
    ;

    ted
    .def("identity", []() { return ec::twisted_edwards::identity(); }, "Returns the affine identity point.")
    .def("ext_identity", []() { return ec::twisted_edwards::ext_identity(); }, "Returns the extended identity point.")
    .def("to_ext", &ec::twisted_edwards::to_ext, py::arg("curve"), py::arg("p"), "Converts an affine point to extended coordinates.")
    .def("to_affine", &ec::twisted_edwards::to_affine, py::arg("curve"), py::arg("p"), "Converts an extended point to affine coordinates.")
    .def("in_curve", py::overload_cast<const ec::twisted_edwards::Curve&, const ec::Point&>(&ec::twisted_edwards::in_curve),
         py::arg("curve"), py::arg("p"), "Checks whether an affine point lies on the curve.")
    .def("in_curve", py::overload_cast<const ec::twisted_edwards::Curve&, const ec::twisted_edwards::ExtPoint&>(&ec::twisted_edwards::in_curve),
         py::arg("curve"), py::arg("p"), "Checks whether an extended point lies on the curve.")
    .def("equivalent", &ec::twisted_edwards::equivalent, py::arg("curve"), py::arg("a"), py::arg("b"),
         "Checks whether two extended points represent the same affine point.")
    .def("add", py::overload_cast<const ec::twisted_edwards::Curve&, const ec::Point&, const ec::Point&>(&ec::twisted_edwards::add),
         py::arg("curve"), py::arg("a"), py::arg("b"), "Adds two affine points.")
    .def("add", py::overload_cast<const ec::twisted_edwards::Curve&, const ec::twisted_edwards::ExtPoint&, const ec::twisted_edwards::ExtPoint&>(&ec::twisted_edwards::add),
         py::arg("curve"), py::arg("a"), py::arg("b"), "Adds two extended points.")
    .def("dbl", py::overload_cast<const ec::twisted_edwards::Curve&, const ec::Point&>(&ec::twisted_edwards::dbl),
         py::arg("curve"), py::arg("p"), "Doubles an affine point.")
    .def("dbl", py::overload_cast<const ec::twisted_edwards::Curve&, const ec::twisted_edwards::ExtPoint&>(&ec::twisted_edwards::dbl),
         py::arg("curve"), py::arg("p"), "Doubles an extended point.")
    .def("mul_scalar", py::overload_cast<const ec::twisted_edwards::Curve&, const ec::Point&, const BigInt&>(&ec::twisted_edwards::mul_scalar),
         py::arg("curve"), py::arg("p"), py::arg("k"), "Multiplies an affine point by a scalar.")
    .def("mul_scalar", py::overload_cast<const ec::twisted_edwards::Curve&, const ec::twisted_edwards::ExtPoint&, const BigInt&>(&ec::twisted_edwards::mul_scalar),
         py::arg("curve"), py::arg("p"), py::arg("k"), "Multiplies an extended point by a scalar.")
    ;
}

#endif
