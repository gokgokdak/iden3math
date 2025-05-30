#pragma once

#include <iden3math/bigint.h>
#include <iden3math/ec/point.h>
#include <iden3math/fp1.h>
#include <iden3math/macro.h>

namespace iden3math::ec::babyjub { // A * x² + y² == 1 + D * x² * y²

API const BigInt& prime();

API const Fp1& finite_field();

API const BigInt& group_order();

API const BigInt& sub_group_order();

API const Point& zero();

API const Point& generator();

API Point add(const Point& a, const Point& b);

API Point mul_scalar(const Point& p, const BigInt& k);

API bool in_sub_group(const Point& p);

API bool in_curve(const Point& p);

API ByteVec1D compress(const Point& p, Endian endian);

API std::optional<Point> decompress(ByteVec1D packed, Endian endian);

} // namespace iden3math::ec::babyjub
