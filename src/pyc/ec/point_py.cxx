#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include "pyc/helper_py.h"
#include <iden3math/ec/point.h>

namespace py = pybind11;

using namespace iden3math;

void init_ec_point(py::module_& m) {
    py::class_<ec::Point>(m, "Point")
    .def(py::init<const BigInt&, const BigInt&>(), py::arg("x") = 0, py::arg("y") = 0, "Creates a point with given x and y coordinates.")
    .def("x", [](const ec::Point& self) { return self.x; }, "Returns the x coordinate of the point.")
    .def("y", [](const ec::Point& self) { return self.y; }, "Returns the y coordinate of the point.")
    .def("__eq__", &ec::Point::operator==, py::arg("other"), "Checks if two points are equal.")
    .def("__ne__", &ec::Point::operator!=, py::arg("other"), "Checks if two points are not equal.")
    ;
}

#endif
