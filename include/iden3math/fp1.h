#pragma once

#include <iden3math/bigint.h>
#include <optional>

namespace iden3math {

// Operation on GF(p^1)
class API Fp1 final {
public:
    Fp1(BigInt prime);
    ~Fp1() = default;

public:
    [[nodiscard]] BigInt                mod_reduce(const BigInt& a) const;
    [[nodiscard]] BigInt                add(const BigInt& a, const BigInt& b) const;
    [[nodiscard]] BigInt                sub(const BigInt& a, const BigInt& b) const;
    [[nodiscard]] BigInt                mul(const BigInt& a, const BigInt& b) const;
    [[nodiscard]] std::optional<BigInt> div(const BigInt& a, const BigInt& b) const;
    [[nodiscard]] std::optional<BigInt> pow(const BigInt& a, const BigInt& b) const;
    [[nodiscard]] BigInt                square(const BigInt& a) const;
    [[nodiscard]] std::optional<BigInt> sqrt(BigInt a) const;
    [[nodiscard]] std::optional<BigInt> mod_inv(const BigInt& a) const;
    [[nodiscard]] BigInt                neg(const BigInt& a) const;
    [[nodiscard]] bool                  has_sqrt(const BigInt& a) const;

private:
    const BigInt p_;
    const uint32_t m_;
    // Tonelli–Shanks square root, (p_ ^ m_) - 1 = q_ * (2 ^ s_)
    int32_t s_;
    BigInt q_;
    BigInt q_m1_d2_;   // (q_ - 1) /  2
    BigInt z_;         // z_ / p_ = -1
};

} // namespace iden3math
