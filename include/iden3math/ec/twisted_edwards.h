#pragma once

#include <iden3math/bigint.h>
#include <iden3math/ec/point.h>
#include <iden3math/fp1.h>
#include <iden3math/macro.h>
#include <optional>
#include <utility>

namespace iden3math::ec::twisted_edwards {

class API Curve final {
public:
    Curve(const Fp1& field, BigInt a, BigInt d)
        : field_(&field), a_(std::move(a)), d_(std::move(d)) {}

    ~Curve() = default;

    [[nodiscard]] const Fp1& field() const { return *field_; }
    [[nodiscard]] const BigInt& a() const { return a_; }
    [[nodiscard]] const BigInt& d() const { return d_; }

private:
    const Fp1* field_;
    BigInt a_;
    BigInt d_;
};

class API ExtPoint final {
public:
    explicit ExtPoint(BigInt X = 0, BigInt Y = 1, BigInt Z = 1, BigInt T = 0)
        : X(std::move(X)), Y(std::move(Y)), Z(std::move(Z)), T(std::move(T)) {}

    ~ExtPoint() = default;
    bool operator==(const ExtPoint& p) const { return X == p.X && Y == p.Y && Z == p.Z && T == p.T; }
    bool operator!=(const ExtPoint& p) const { return X != p.X || Y != p.Y || Z != p.Z || T != p.T; }

public:
    BigInt X;
    BigInt Y;
    BigInt Z;
    BigInt T;
};

API const Point& identity();

API const ExtPoint& ext_identity();

API ExtPoint to_ext(const Curve& curve, const Point& p);

API std::optional<Point> to_affine(const Curve& curve, const ExtPoint& p);

API bool in_curve(const Curve& curve, const Point& p);

API bool in_curve(const Curve& curve, const ExtPoint& p);

API bool equivalent(const Curve& curve, const ExtPoint& a, const ExtPoint& b);

API Point add(const Curve& curve, const Point& a, const Point& b);

API ExtPoint add(const Curve& curve, const ExtPoint& a, const ExtPoint& b);

API Point dbl(const Curve& curve, const Point& p);

API ExtPoint dbl(const Curve& curve, const ExtPoint& p);

API Point mul_scalar(const Curve& curve, const Point& p, const BigInt& k);

API ExtPoint mul_scalar(const Curve& curve, const ExtPoint& p, const BigInt& k);

} // namespace iden3math::ec::twisted_edwards
