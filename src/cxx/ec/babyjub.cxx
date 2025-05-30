#include <iden3math/ec/babyjub.h>
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
    Point r;
    auto beta = f().mul(a.x, b.y);
    auto gamma = f().mul(a.y, b.x);
    auto delta = f().mul(
        f().sub(a.y, f().mul(A, a.x)),
        f().add(b.x, b.y)
    );
    auto tau = f().mul(beta, gamma);
    auto dtau = f().mul(D, tau);
    r.x = *f().div(
        f().add(beta, gamma),
        f().add(1, dtau)
    );
    r.y = *f().div(
        f().add(delta, f().sub(f().mul(A, beta), gamma)),
        f().sub(1, dtau)
    );
    return r;
}

Point mul_scalar(const Point& p, const BigInt& k) {
    Point r;
    r.x = 0;
    r.y = 1;
    BigInt rem = k;
    Point exp = p;
    while (0 != rem) {
        if (rem.odd()) {
            r = add(r, exp);
        }
        exp = add(exp, exp);
        rem >>= 1;
    }
    return r;
}

bool in_sub_group(const Point& p) {
    if (!in_curve(p)) {
        return false;
    }
    auto r = mul_scalar(p, sub_group_order());
    return 0 == r.x && 1 == r.y;
}

bool in_curve(const Point& p) {
    auto x2 = *f().pow(p.x, 2); // Always not std::nullopt because exp is 2 >= 0
    auto y2 = *f().pow(p.y, 2); // Always not std::nullopt because exp is 2 >= 0
    auto lhs = f().add(f().mul(A, x2), y2); // A * x² + y²
    auto rhs =  f().add(1, f().mul(f().mul(x2, y2), D)); // 1 + D * x² * y²
    return lhs == rhs;
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
