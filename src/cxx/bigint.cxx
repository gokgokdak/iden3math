#include <iden3math/serialize.h>
#include <iden3math/bigint.h>
#include <gmpxx.h>

namespace iden3math {

class BigInt::Impl : public mpz_class {
    using mpz_class::mpz_class;
};

BigInt::BigInt(int32_t num) : impl_(new Impl(num)) {}

BigInt::BigInt(uint32_t num) : impl_(new Impl(num)) {}

BigInt::BigInt(int64_t num) : impl_(new Impl()) {
    bool sign = num < 0;
    num = std::abs(num);
    mpz_import(impl_->get_mpz_t(), 1, -1, sizeof(num), 0, 0, &num);
    if (sign) {
        mpz_neg(impl_->get_mpz_t(), impl_->get_mpz_t());
    }
}

BigInt::BigInt(uint64_t num) {
    mpz_import(impl_->get_mpz_t(), 1, -1, sizeof(num), 0, 0, &num);
}

BigInt::BigInt(const ByteVec1D& bytes, Endian endian) : impl_(new Impl()) {
    if (bytes.empty()) {
        *impl_ = 0;
        return;
    }
    mpz_import(impl_->get_mpz_t(), bytes.size(), endian == BE ? 1 : -1, sizeof(uint8_t), 0, 0, bytes.data());
}

BigInt::BigInt(const std::string& num, int32_t base) : impl_(new Impl(num, base)) {}

BigInt::BigInt(const Impl& other) : impl_(std::make_unique<Impl>(other)) {}

BigInt::BigInt(const BigInt& other) {
    impl_ = std::make_unique<Impl>(*other.impl_);
}

BigInt::BigInt(BigInt&& other) noexcept {
    impl_ = std::move(other.impl_);
}

BigInt::~BigInt() { impl_.reset(); }

bool BigInt::odd() const { return mpz_odd_p(impl_->get_mpz_t()); }

bool BigInt::even() const { return mpz_even_p(impl_->get_mpz_t()); }

std::string BigInt::str(uint8_t base) const { return impl_->get_str(base); }

uint32_t BigInt::unsigned_int() const {
    if (*impl_ > UINT32_MAX) {
        throw std::out_of_range("BigInt is too large to fit in a uint32_t");
    }
    return impl_->get_ui();
}

BitVec1D BigInt::bits(Endian endian) const {
    return serialize::bytes_to_bits(bytes(endian));
}

ByteVec1D BigInt::bytes(Endian endian) const {
    ByteVec1D out(bytes_size());
    size_t len = 0;
    mpz_export(out.data(), &len, endian == BE ? 1 : -1, sizeof(uint8_t), 0, 0, impl_->get_mpz_t());
    out.resize(len);
    return out;
}

size_t BigInt::bits_size() const {
    return mpz_sizeinbase(impl_->get_mpz_t(), 2);
}

size_t BigInt::bytes_size() const {
    return (bits_size() + 7) / 8;
}

BigInt BigInt::pow(const uint32_t& exp) const {
    BigInt r;
    mpz_pow_ui(r.impl_->get_mpz_t(), impl_->get_mpz_t(), exp);
    return r;
}

BigInt BigInt::pow_mod(const BigInt& exp, const BigInt& modulo) const {
    BigInt r;
    mpz_powm(r.impl_->get_mpz_t(), impl_->get_mpz_t(), exp.impl_->get_mpz_t(), modulo.impl_->get_mpz_t());
    return r;
}

std::optional<BigInt> BigInt::mod_inv(const BigInt& modulo) const {
    Impl inv;
    if (0 == mpz_invert(inv.get_mpz_t(), impl_->get_mpz_t(), modulo.impl_->get_mpz_t())) {
        return std::nullopt;
    }
    return inv;
}

BigInt& BigInt::operator=(const BigInt& other) {
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }
    return *this;
}

BigInt& BigInt::operator=(BigInt&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

#define BIGINT_OP_IMPL_ARITHMETIC_BITWISE(op) \
    BigInt BigInt::operator op(const BigInt& rhs) const { return {*impl_ op *rhs.impl_}; } \
    BigInt BigInt::operator op(int32_t rhs) const { return {*impl_ op rhs}; } \
    BigInt BigInt::operator op(uint32_t rhs) const { return {*impl_ op rhs}; } \
    BigInt BigInt::operator op(int64_t rhs) const { return {*this op BigInt(rhs)}; } \
    BigInt BigInt::operator op(uint64_t rhs) const { return {*this op BigInt(rhs)}; } \
    void BigInt::operator op##=(const BigInt& rhs) { *impl_ op##= *rhs.impl_; } \
    void BigInt::operator op##=(int32_t rhs) { *impl_ op##= rhs; } \
    void BigInt::operator op##=(uint32_t rhs) { *impl_ op##= rhs; } \
    void BigInt::operator op##=(int64_t rhs) { *this op##= BigInt(rhs); } \
    void BigInt::operator op##=(uint64_t rhs) { *this op##= BigInt(rhs); }

#define BIGINT_OP_IMPL_BIT_SHIFT(op) \
    BigInt BigInt::operator op(uint32_t rhs) const { return {mpz_class(*impl_) op rhs}; } \
    void BigInt::operator op##=(uint32_t rhs) { *impl_ op##= rhs; }

#define BIGINT_OP_IMPL_LOGIC(op) \
    bool BigInt::operator op(const BigInt& rhs) const { return *impl_ op *rhs.impl_; } \
    bool BigInt::operator op(int32_t rhs) const { return *impl_ op rhs; } \
    bool BigInt::operator op(uint32_t rhs) const { return *impl_ op rhs; } \
    bool BigInt::operator op(int64_t rhs) const { return *this op BigInt(rhs); } \
    bool BigInt::operator op(uint64_t rhs) const { return *this op BigInt(rhs); }

BIGINT_OP_IMPL_ARITHMETIC_BITWISE(+)
BIGINT_OP_IMPL_ARITHMETIC_BITWISE(-)
BIGINT_OP_IMPL_ARITHMETIC_BITWISE(*)
BIGINT_OP_IMPL_ARITHMETIC_BITWISE(/)
BIGINT_OP_IMPL_ARITHMETIC_BITWISE(%)
BIGINT_OP_IMPL_ARITHMETIC_BITWISE(&)
BIGINT_OP_IMPL_ARITHMETIC_BITWISE(|)
BIGINT_OP_IMPL_ARITHMETIC_BITWISE(^)
BIGINT_OP_IMPL_BIT_SHIFT(<<)
BIGINT_OP_IMPL_BIT_SHIFT(>>)
BIGINT_OP_IMPL_LOGIC(==)
BIGINT_OP_IMPL_LOGIC(!=)
BIGINT_OP_IMPL_LOGIC(<)
BIGINT_OP_IMPL_LOGIC(>)
BIGINT_OP_IMPL_LOGIC(<=)
BIGINT_OP_IMPL_LOGIC(>=)

BigInt& BigInt::operator++() {
    ++(*impl_);
    return *this;
}

BigInt BigInt::operator++(int) const {
    BigInt temp(*this);
    ++(*impl_);
    return temp;
}

BigInt& BigInt::operator--() {
    --(*impl_);
    return *this;
}

BigInt BigInt::operator--(int) const {
    BigInt temp(*this);
    --(*impl_);
    return temp;
}

BigInt BigInt::operator+() const { return *this; }

BigInt BigInt::operator-() const { return {-*impl_}; }

BigInt BigInt::operator~() const { return {~*impl_}; }

bool BigInt::operator!() const { return 0 == *impl_; }

BigInt::operator bool() const { return 0 != *impl_; }

} // namespace iden3math
