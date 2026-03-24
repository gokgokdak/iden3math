#include <iden3math/ec/twisted_edwards.h>
#include <array>

namespace iden3math::ec::twisted_edwards {

ExtPoint neg(const Curve& curve, const ExtPoint& p) {
    const auto& F = curve.field();
    return ExtPoint(F.neg(p.X), p.Y, p.Z, F.neg(p.T));
}

const Point& identity() {
    static const Point IDENTITY(0, 1);
    return IDENTITY;
}

const ExtPoint& ext_identity() {
    static const ExtPoint IDENTITY(0, 1, 1, 0);
    return IDENTITY;
}

ExtPoint to_ext(const Curve& curve, const Point& p) {
    return ExtPoint(p.x, p.y, 1, curve.field().mul(p.x, p.y));
}

std::optional<Point> to_affine(const Curve& curve, const ExtPoint& p) {
    auto z_inv = curve.field().mod_inv(p.Z);
    if (std::nullopt == z_inv) {
        return std::nullopt;
    }
    return Point(
        curve.field().mul(p.X, *z_inv),
        curve.field().mul(p.Y, *z_inv)
    );
}

bool in_curve(const Curve& curve, const Point& p) {
    const auto&  F = curve.field();
    const auto  x2 = F.square(p.x);
    const auto  y2 = F.square(p.y);
    const auto lhs = F.add(F.mul(curve.a(), x2), y2);
    const auto rhs = F.add(1, F.mul(F.mul(x2, y2), curve.d()));
    return lhs == rhs;
}

bool in_curve(const Curve& curve, const ExtPoint& p) {
    if (0 == p.Z) {
        return false;
    }
    const auto& F = curve.field();
    if (F.mul(p.X, p.Y) != F.mul(p.Z, p.T)) {
        return false;
    }
    const auto  x2 = F.square(p.X);
    const auto  y2 = F.square(p.Y);
    const auto  z2 = F.square(p.Z);
    const auto  t2 = F.square(p.T);
    const auto lhs = F.add(F.mul(curve.a(), x2), y2);
    const auto rhs = F.add(z2, F.mul(curve.d(), t2));
    return lhs == rhs;
}

bool equivalent(const Curve& curve, const ExtPoint& a, const ExtPoint& b) {
    if (0 == a.Z || 0 == b.Z) {
        return false;
    }
    const auto& F = curve.field();
    return F.mul(a.X, b.Z) == F.mul(b.X, a.Z) && F.mul(a.Y, b.Z) == F.mul(b.Y, a.Z);
}

ExtPoint add(const Curve& curve, const ExtPoint& p1, const ExtPoint& p2) {
    const auto& F = curve.field();
    const auto xx = F.mul(p1.X, p2.X);
    const auto yy = F.mul(p1.Y, p2.Y);
    const auto tt = F.mul(curve.d(), F.mul(p1.T, p2.T));
    const auto zz = F.mul(p1.Z, p2.Z);
    const auto  e = F.sub(
        F.mul(F.add(p1.X, p1.Y), F.add(p2.X, p2.Y)),
        F.add(xx, yy)
    );
    const auto ff = F.sub(zz, tt);
    const auto  g = F.add(zz, tt);
    const auto  h = F.sub(yy, F.mul(curve.a(), xx));
    return ExtPoint(
        F.mul(e, ff),
        F.mul(g, h),
        F.mul(ff, g),
        F.mul(e, h)
    );
}

Point add(const Curve& curve, const Point& a, const Point& b) {
    return *to_affine(curve, add(curve, to_ext(curve, a), to_ext(curve, b)));
}

ExtPoint dbl(const Curve& curve, const ExtPoint& p) {
    const auto&  F = curve.field();
    const auto  xx = F.square(p.X);
    const auto  yy = F.square(p.Y);
    const auto  zz = F.square(p.Z);
    const auto   c = F.add(zz, zz);
    const auto axx = F.mul(curve.a(), xx);
    const auto   e = F.sub(F.square(F.add(p.X, p.Y)), F.add(xx, yy));
    const auto   g = F.add(axx, yy);
    const auto  ff = F.sub(g, c);
    const auto   h = F.sub(axx, yy);
    return ExtPoint(
        F.mul(e, ff),
        F.mul(g, h),
        F.mul(ff, g),
        F.mul(e, h)
    );
}

Point dbl(const Curve& curve, const Point& p) {
    return *to_affine(curve, dbl(curve, to_ext(curve, p)));
}

ExtPoint mul_scalar(const Curve& curve, const ExtPoint& p, const BigInt& k) {
    if (0 == k) {
        return ext_identity();
    }
    if (k < 0) {
        return mul_scalar(curve, neg(curve, p), -k);
    }
    constexpr int W = 4;
    constexpr int TABLE_SIZE = 1 << W;

    std::array<ExtPoint, TABLE_SIZE> table;
    table[0] = ext_identity();
    table[1] = p;
    for (int i = 2; i < TABLE_SIZE; ++i) {
        table[i] = add(curve, table[i - 1], table[1]);
    }
    auto scalar_bytes = k.bytes(LE);
    size_t n_bits = k.bits_size();
    int n_windows = static_cast<int>((n_bits + W - 1) / W);

    ExtPoint r = ext_identity();
    for (int i = n_windows - 1; i >= 0; --i) {
        for (int j = 0; j < W; ++j) {
            r = dbl(curve, r);
        }
        uint32_t wval = 0;
        for (int j = 0; j < W; ++j) {
            size_t bit_pos = static_cast<size_t>(i) * W + j;
            size_t byte_idx = bit_pos / 8;
            size_t bit_idx = bit_pos % 8;
            if (byte_idx < scalar_bytes.size()) {
                wval |= (((scalar_bytes[byte_idx] >> bit_idx) & 1u) << j);
            }
        }
        if (0 != wval) {
            r = add(curve, r, table[wval]);
        }
    }
    return r;
}

Point mul_scalar(const Curve& curve, const Point& p, const BigInt& k) {
    return *to_affine(curve, mul_scalar(curve, to_ext(curve, p), k));
}

} // namespace iden3math::ec::twisted_edwards
