#include <iden3math/fp1.h>
#include <iden3math/random.h>
#include <cassert>
#include <utility>
#include <gmpxx.h>

namespace iden3math {

Fp1::Fp1(BigInt prime)
    : p_(std::move(prime))
    , m_(1)
{
    // Tonelli–Shanks square root
    assert(p_.odd()); // Prime must be odd
    BigInt tmp = p_.pow(m_);
    // Init: q_ = p_ ^ m_ - 1
    q_ = tmp - 1;
    s_ = 0;
    while (q_.even()) {
        s_ += 1;
        q_ /= 2;
    }
    // Final: q_ = (p_ ^ m_ - 1) / (2 ^ s_)
    q_m1_d2_ = (q_ - 1) / 2;
    auto two_pow_s_m1 = BigInt(2).pow(s_ - 1);
    // Find a z_ satisfy z_ / p_ = -1
    BigInt c0 = 1;
    while (1 == c0) {
        BigInt rand = random::get_integer(p_);
        z_ = rand.pow_mod(q_, p_); // z_ = rand ^ q_
        c0 = z_.pow_mod(two_pow_s_m1, p_); // c0 = z_ ^ (2 ^ (s_ - 1))
    }
}

BigInt Fp1::mod_reduce(const BigInt& a) const {
    BigInt r = a % p_;
    if (r < 0) {
        r += p_;
    }
    return r;
}

BigInt Fp1::add(const BigInt& a, const BigInt& b) const {
    BigInt r = (a + b) % p_;
    if (r < 0) {
        r += p_;
    }
    return r;
}

BigInt Fp1::sub(const BigInt& a, const BigInt& b) const {
    return (a % p_ - b % p_ + p_) % p_;
}

mpz_class mul(const mpz_class& a, const mpz_class& b) {
    return (a % a - b % a + a) % a;
}

BigInt Fp1::mul(const BigInt& a, const BigInt& b) const {
    BigInt r = (a * b) % p_;
    if (r < 0) {
        r += p_;
    }
    return r;
}

std::optional<BigInt> Fp1::div(const BigInt& a, const BigInt& b) const {
    if (0 == b) {
        return std::nullopt;
    }
    auto inv_b = mod_inv(b);
    if (std::nullopt == inv_b) {
        return std::nullopt;
    }
    return mod_reduce(a) * (*inv_b) % p_;
}

std::optional<BigInt> Fp1::pow(const BigInt& a, const BigInt& b) const {
    BigInt base = mod_reduce(a);
    BigInt r;
    if (b >= 0) {
        r = base.pow_mod(b, p_);
    } else {
        // r = a_inv ^ |b|
        auto base_inv = mod_inv(base);
        if (std::nullopt == base_inv) {
            return std::nullopt;
        }
        BigInt abs_exp = -b;
        r = base_inv->pow_mod(abs_exp, p_);
    }
    return r;
}

BigInt Fp1::square(const BigInt& a) const {
    return mul(a, a);
}

// Tonelli–Shanks
std::optional<BigInt> Fp1::sqrt(BigInt a) const {
    a = mod_reduce(a);
    if (0 == a) {
        return 0;
    }
    if (!has_sqrt(a)) {
        return std::nullopt;
    }
    auto opt_w = pow(a, q_m1_d2_);
    if (std::nullopt == opt_w) {
        return std::nullopt;
    }
    auto w = *opt_w;
    BigInt v = s_;
    BigInt z = z_;
    BigInt x = mul(a, w);
    BigInt b = mul(x, w);
    while (1 != b) {
        BigInt b2k = square(b);
        BigInt k = 1;
        while (1 != b2k) {
            b2k = square(b2k);
            ++k;
        }
        w = z;
        for (BigInt i = 0; i < v - k - 1; ++i) {
            w = square(w);
        }
        z = square(w);
        b = mul(b, z);
        x = mul(x, w);
        v = k;
    }
    return x;
}

std::optional<BigInt> Fp1::mod_inv(const BigInt& a) const {
    return mod_reduce(a).mod_inv(p_);
}

BigInt Fp1::neg(const BigInt& a) const {
    BigInt r = mod_reduce(a);
    return r == 0 ? r : (p_ - r);
}

bool Fp1::has_sqrt(const BigInt& a) const {
    BigInt exp = (p_ - 1) / 2;
    const auto r = pow(a, exp);
    if (std::nullopt == r) {
        return false;
    }
    return *r == 1;
}

} // namespace iden3math
