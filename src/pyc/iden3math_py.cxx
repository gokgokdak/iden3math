#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include "pyc/ec/ec_py.h"
#include "pyc/hash/hash_py.h"

namespace py = pybind11;

void init_typedef(py::module_&);
void init_bitvec(py::module_&);
void init_fp1(py::module_&);
void init_prime(py::module_&);
void init_random(py::module_&);

PYBIND11_MODULE(iden3math, m) {
    init_typedef(m);
    init_bitvec(m);
    init_fp1(m);
    init_prime(m);
    init_random(m);
    init_ec(m);
    init_hash(m);
}

#endif
