#include <iden3math/ec/babyjub.h>
#include <iden3math/ec/twisted_edwards.h>
#include <iden3math/prime.h>
#include <iden3math/serialize.h>

namespace iden3math::ec::babyjub {

inline bool non_regulated_x(const BigInt& x) {
    return x > (prime::bn254() - 1) / 2;
}

static const BigInt A("168700", 10);

static const BigInt D("168696", 10);

const BigInt& prime() { return prime::bn254(); }

const Fp1& finite_field() {
    static const Fp1 F(prime::bn254());
    return F;
}

const twisted_edwards::Curve& curve() {
    static const twisted_edwards::Curve CURVE(finite_field(), A, D);
    return CURVE;
}

const BigInt& group_order() {
    static const BigInt GROUP_ORDER("21888242871839275222246405745257275088614511777268538073601725287587578984328", 10);
    return GROUP_ORDER;
}

const BigInt& sub_group_order() {
    static const BigInt SUB_GROUP_ORDER = group_order() / 8;
    return SUB_GROUP_ORDER;
}

const Point& zero() {
    static const Point ZERO(0, 1);
    return ZERO;
}

const Point& generator() {
    static const Point GENERATOR(
        BigInt("995203441582195749578291179787384436505546430278305826713579947235728471134", 10),
        BigInt("5472060717959818805561601436314318772137091100104008585924551046643952123905", 10)
    );
    return GENERATOR;
}

inline const Fp1& f() { // Shorter name for convenience
    return finite_field();
}

Point add(const Point& a, const Point& b) {
    return twisted_edwards::add(curve(), a, b);
}

Point mul_scalar(const Point& p, const BigInt& k) {
    return twisted_edwards::mul_scalar(curve(), p, k);
}

bool in_sub_group(const Point& p) {
    if (!in_curve(p)) {
        return false;
    }
    auto r = mul_scalar(p, sub_group_order());
    return 0 == r.x && 1 == r.y;
}

bool in_curve(const Point& p) {
    return twisted_edwards::in_curve(curve(), p);
}

ByteVec1D compress(const Point& p, Endian endian) {
    auto bytes = p.y.bytes(endian);
    serialize::pad(bytes, 0x00, 32 - bytes.size(), BE == endian);;
    if (non_regulated_x(p.x)) {
        auto& sign_byte = LE == endian ? bytes.back() : bytes.front();
        sign_byte |= 0x80;
    }
    return bytes;
}

std::optional<Point> decompress(ByteVec1D packed, Endian endian) {
    auto& sign_byte = LE == endian ? packed.back() : packed.front();
    const bool sign = (sign_byte & 0x80) >> 7;
    if (sign) {
        sign_byte &= 0x7f;
    }
    auto num = BigInt(packed, endian);
    if (num >= prime::bn254()) {
        return std::nullopt;
    }
    Point P;
    P.y = num;
    auto y2 = f().square(P.y);
    auto x = f().sqrt(
        *f().div(
            f().sub(1, y2),
            f().sub(A, f().mul(D, y2))
        )
    );
    if (std::nullopt == x) {
        return std::nullopt;
    }
    if (sign != non_regulated_x(*x)) {
        x = f().neg(*x);
    }
    P.x = *x;
    if (!in_curve(P)) {
        return std::nullopt;
    }
    return P;
}

} // namespace iden3math::ec::babyjub
