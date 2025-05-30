#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pyc/helper_py.h"
#include <iden3math/ec/babyjub.h>

namespace py = pybind11;

using namespace iden3math;

void init_ec_babyjub(py::module_& m) {
    m.def_submodule("babyjub")
    .def("prime", &ec::babyjub::prime, "Returns the prime number of the BabyJubjub curve.")
    .def("finite_field", &ec::babyjub::finite_field, "Returns the finite field of the BabyJubjub curve.")
    .def("group_order", &ec::babyjub::group_order, "Returns the order of the group of points on the BabyJubjub curve.")
    .def("sub_group_order", &ec::babyjub::sub_group_order, "Returns the order of the subgroup of points on the BabyJubjub curve.")
    .def("zero", &ec::babyjub::zero, "Returns the zero point (identity element) of the BabyJubjub curve.")
    .def("generator", &ec::babyjub::generator, "Returns the generator point of the BabyJubjub curve.")
    .def("add", &ec::babyjub::add, py::arg("a"), py::arg("b"), "Adds two points on the BabyJubjub curve.")
    .def("mul_scalar", &ec::babyjub::mul_scalar, py::arg("p"), py::arg("k"), "Multiplies a point on the BabyJubjub curve by a scalar.")
    .def("in_sub_group", &ec::babyjub::in_sub_group, py::arg("p"), "Checks if a point is in the subgroup of the BabyJubjub curve.")
    .def("in_curve", &ec::babyjub::in_curve, py::arg("p"), "Checks if a point is on the BabyJubjub curve.")
    .def("compress", &ec::babyjub::compress, py::arg("p"), py::arg("endian"), "Compresses a point on the BabyJubjub curve into a byte vector.")
    .def("decompress", &ec::babyjub::decompress, py::arg("packed"), py::arg("endian"), "Decompresses a byte vector into a point on the BabyJubjub curve.")
    ;
}

#endif
