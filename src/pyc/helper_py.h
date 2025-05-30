#pragma once

#ifdef IDEN3MATH_BUILD_PY

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iden3math/bigint.h>

namespace py = pybind11;

using namespace iden3math;

namespace pybind11::detail {

template <>
class type_caster<BigInt> {
    PYBIND11_TYPE_CASTER(BigInt, _("BigInt"));
    bool load(handle src, bool) {
        auto v = py::cast<py::int_>(src);
        auto s = py::str(v);
        value = {s, 10};
        return true;
    }
    static handle cast(const BigInt& v, return_value_policy, handle) {
        return py::int_(py::str(v.str(10))).release();
    }
};

template <>
class type_caster<ByteVec1D> {
    PYBIND11_TYPE_CASTER(ByteVec1D, _("ByteVec1D"));
    bool load(handle src, bool) {
        if (PyByteArray_Check(src.ptr())) {
            auto* data = reinterpret_cast<Byte*>(PyByteArray_AsString(src.ptr()));
            auto len = PyByteArray_Size(src.ptr());
            value = ByteVec1D(data, data + len);
            return true;
        }
        if (PyBytes_Check(src.ptr())) {
            auto* data = reinterpret_cast<Byte*>(PyBytes_AsString(src.ptr()));
            auto len = PyBytes_Size(src.ptr());
            value = ByteVec1D(data, data + len);
            return true;
        }
        return false;
    }
    static handle cast(const ByteVec1D& v, return_value_policy, handle) {
        return py::bytearray(reinterpret_cast<const char*>(v.data()), v.size()).release();
    }
};

} // namespace pybind11::detail

#endif
