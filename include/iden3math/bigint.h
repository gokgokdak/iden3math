#pragma once

#include <iden3math/bitvec.h>
#include <iden3math/typedef.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#define IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(op) \
    BigInt operator op(const BigInt& rhs) const; \
    BigInt operator op(int32_t rhs) const; \
    BigInt operator op(uint32_t rhs) const; \
    BigInt operator op(int64_t rhs) const; \
    BigInt operator op(uint64_t rhs) const; \
    void operator op##=(const BigInt& rhs); \
    void operator op##=(int32_t rhs); \
    void operator op##=(uint32_t rhs); \
    void operator op##=(int64_t rhs); \
    void operator op##=(uint64_t rhs); \
    template <typename T, enable_if_int_signed_t<T> = 0> \
        friend BigInt operator op(const T& a, const BigInt& b) { return BigInt(a) op b; } \
    template <typename T, enable_if_int_unsigned_t<T> = 0> \
        friend BigInt operator op(const T& a, const BigInt& b) { return BigInt(a) op b; }

#define IDEN3MATH_BIGINT_OP_BIT_SHIFT(op) \
    BigInt operator op(const BigInt& rhs) const; \
    BigInt operator op(uint32_t rhs) const; \
    void operator op##=(const BigInt& rhs); \
    void operator op##=(uint32_t rhs); \
    template <typename T, enable_if_int_t<T> = 0> \
        friend BigInt operator op(const BigInt& a, const T& b) { return a op b; } \

#define IDEN3MATH_BIGINT_OP_LOGIC(op) \
    bool operator op(const BigInt& rhs) const; \
    bool operator op(int32_t rhs) const; \
    bool operator op(uint32_t rhs) const; \
    bool operator op(int64_t rhs) const; \
    bool operator op(uint64_t rhs) const; \
    template <typename T, enable_if_int_t<T> = 0> \
        friend bool operator op(const T& a, const BigInt& b)  { return BigInt(a) op b; } \
    template <typename T, enable_if_int_t<T> = 0> \
        friend bool operator op(const BigInt& a, const T& b)  { return a op BigInt(b); }

namespace iden3math {

class API BigInt {
public:
    // Forward declaration of the internal implementation class
    class Impl;

    // Native int constructors
    BigInt(int32_t num = 0);
    explicit BigInt(uint32_t num);
    explicit BigInt(int64_t num);
    explicit BigInt(uint64_t num);
    template <typename T, enable_if_int_signed_t<T> = 0>
    explicit BigInt(const T& num) : BigInt(num) {}
    template <typename T, enable_if_int_unsigned_t<T> = 0>
    explicit BigInt(const T& num) : BigInt(num) {}

    // String and byte array constructors
    /**
     * Convert bytes to a big integer
     * @param   bytes   Byte array to convert
     * @param   endian  LITTLE: (Low Addr){0x11, 0x22, 0x00, 0xff}(High Addr) -> 0xffee002211
     *                  BIG:   (High Addr){0xff, 0xee, 0x00, 0x22}(Low Addr) -> 0xffee002211
     */
    BigInt(const ByteVec1D& bytes, Endian endian);
    BigInt(const std::string& num, int32_t base);

    // Copy && move constructors
    BigInt(const Impl& other);
    BigInt(const BigInt& other);
    BigInt(BigInt&& other) noexcept;

    // Destructor
    ~BigInt();

public:
    [[nodiscard]] bool odd() const;
    [[nodiscard]] bool even() const;
    [[nodiscard]] std::string str(uint8_t base = 16) const;
    [[nodiscard]] uint32_t unsigned_int() const;
    /**
     * Convert an integer to a bit array
     * @param   endian  LITTLE: (Low Addr){0x11, 0x22, 0x00, 0xff}(High Addr) -> 0xffee002211
     *                  BIG:   (High Addr){0xff, 0xee, 0x00, 0x22}(Low Addr) -> 0xffee002211
     */
    [[nodiscard]] BitVec1D bits(Endian endian) const;
    /**
     * Convert an integer to a byte array
     * @param   endian  LITTLE: (Low Addr){0x11, 0x22, 0x00, 0xff}(High Addr) -> 0xffee002211
     *                  BIG:   (High Addr){0xff, 0xee, 0x00, 0x22}(Low Addr) -> 0xffee002211
     */
    [[nodiscard]] ByteVec1D bytes(Endian endian) const;
    [[nodiscard]] size_t bits_size() const; // Faster than bits().size()
    [[nodiscard]] size_t bytes_size() const; // Faster than bytes().size()
    [[nodiscard]] BigInt pow(const uint32_t& exp) const;
    [[nodiscard]] BigInt pow_mod(const BigInt& exp, const BigInt& modulo) const;
    [[nodiscard]] std::optional<BigInt> mod_inv(const BigInt& modulo) const;

public:
    BigInt& operator=(const BigInt& other);
    BigInt& operator=(BigInt&& other) noexcept;
    IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(+)
    IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(-)
    IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(*)
    IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(/)
    IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(%)
    IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(&)
    IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(|)
    IDEN3MATH_BIGINT_OP_ARITHMETIC_BITWISE(^)
    IDEN3MATH_BIGINT_OP_BIT_SHIFT(<<)
    IDEN3MATH_BIGINT_OP_BIT_SHIFT(>>)
    IDEN3MATH_BIGINT_OP_LOGIC(==)
    IDEN3MATH_BIGINT_OP_LOGIC(!=)
    IDEN3MATH_BIGINT_OP_LOGIC(<)
    IDEN3MATH_BIGINT_OP_LOGIC(>)
    IDEN3MATH_BIGINT_OP_LOGIC(<=)
    IDEN3MATH_BIGINT_OP_LOGIC(>=)
    BigInt& operator++();
    BigInt  operator++(int) const;
    BigInt& operator--();
    BigInt  operator--(int) const;
    BigInt  operator+() const;
    BigInt  operator-() const;
    BigInt  operator~() const;
    bool    operator!() const;
    explicit operator bool() const;
    friend std::ostream& operator<<(std::ostream& os, const BigInt& a) { return os << a.str(); }

private:
    std::unique_ptr<Impl> impl_;
};

} // namespace iden3math
