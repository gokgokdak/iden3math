#include <iden3math/ec/babyjub.h>
#include <iden3math/fp1.h>
#include <gtest/gtest.h>
#include "helper.h"

namespace iden3math {

std::once_flag PRIMES_ONCE_FLAG;

static BigInt PRIME_17;

static BigInt PRIME_MERSENNE;

static BigInt PRIME_BN254;

static std::vector<BigInt> PRIMES;

void init() {
    std::call_once(PRIMES_ONCE_FLAG, [] {
        // 17
        PRIME_17 = 17;
        PRIMES.emplace_back(PRIME_17);
        // Mersenne
        PRIME_MERSENNE = BigInt(2).pow(31);
        PRIME_MERSENNE -= 1;
        PRIMES.push_back(PRIME_MERSENNE);
        // bn254
        PRIME_BN254 = ec::babyjub::prime();
        PRIMES.push_back(PRIME_BN254);
    });
    EXPECT_EQ(3, PRIMES.size());
}

TEST(fp1, mod_reduce) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(F.mod_reduce(0), 0);
        EXPECT_EQ(F.mod_reduce(1), 1);
        EXPECT_EQ(F.mod_reduce(-1), prime - 1);
        EXPECT_EQ(F.mod_reduce(prime - 1), prime - 1);
        EXPECT_EQ(F.mod_reduce(prime), 0);
        EXPECT_EQ(F.mod_reduce(prime + 1), 1);
        EXPECT_EQ(F.mod_reduce(-prime), 0);
        EXPECT_EQ(F.mod_reduce(-prime - 1), prime - 1);
        EXPECT_EQ(F.mod_reduce(-prime + 1), 1);
    }
}

TEST(fp1, add) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(F.add(0, 0), 0);
        EXPECT_EQ(F.add(2, 3), 5);
        EXPECT_EQ(F.add(-2, 3), 1);
        EXPECT_EQ(F.add(2, -3), prime - 1);
        EXPECT_EQ(F.add(-2, -3), prime - 5);
        EXPECT_EQ(F.add(prime, 0), 0);
        EXPECT_EQ(F.add(prime, 1), 1);
        EXPECT_EQ(F.add(prime - 1, 1), 0);
        EXPECT_EQ(F.add(prime - 2, 5), 3);
        EXPECT_EQ(F.add(prime - 2, -5), prime - 7);
    }
}

TEST(fp1, sub) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(F.sub(0, 0), 0);
        EXPECT_EQ(F.sub(5, 3), 2);
        EXPECT_EQ(F.sub(3, 5), prime - 2);
        EXPECT_EQ(F.sub(-5, 3), prime - 8);
        EXPECT_EQ(F.sub(-3, 5), prime - 8);
        EXPECT_EQ(F.sub(prime, 0), 0);
        EXPECT_EQ(F.sub(prime, 1), prime - 1);
        EXPECT_EQ(F.sub(prime + 1, 1), 0);
        EXPECT_EQ(F.sub(prime + 2, 3), prime - 1);
        EXPECT_EQ(F.sub(prime - 1, 1), prime - 2);
        EXPECT_EQ(F.sub(prime - 2, 5), prime - 7);
        EXPECT_EQ(F.sub(prime - 2, -5), 3);
    }
}

TEST(fp1, mul) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(F.mul(0, 0), 0);
        EXPECT_EQ(F.mul(2, 3), 6);
        EXPECT_EQ(F.mul(-2, 3), prime - 6);
        EXPECT_EQ(F.mul(2, -3), prime - 6);
        EXPECT_EQ(F.mul(-2, -3), 6);
        EXPECT_EQ(F.mul(prime, 0), 0);
        EXPECT_EQ(F.mul(prime, 1), 0);
        EXPECT_EQ(F.mul(prime + 1, 2), 2);
        EXPECT_EQ(F.mul(prime + 2, 3), 6);
        EXPECT_EQ(F.mul(prime - 1, 2), prime - 2);
        EXPECT_EQ(F.mul(prime - 2, 3), prime - 6);
        EXPECT_EQ(F.mul(0, 5), 0);
        EXPECT_EQ(F.mul(-1, 5), prime - 5);
    }
}

TEST(fp1, div) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(std::nullopt, F.div(1, 0));
        EXPECT_EQ(F.div(1, prime - 1), prime - 1);
        EXPECT_EQ(F.div(1, 1), 1);
        EXPECT_EQ(F.div(4, 2), 2);
        EXPECT_EQ(F.div(6, 2), 3);
        EXPECT_EQ(F.div(6, 3), 2);
        EXPECT_EQ(F.div(8, 4), 2);
        EXPECT_EQ(F.div(8, -4), prime - 2);
        EXPECT_EQ(F.div(-8, 4), prime - 2);
        EXPECT_EQ(F.div(-8, -4), 2);
        std::optional<BigInt> x;
        // 5 / 3
        x = F.div(5, 3);
        EXPECT_NE(std::nullopt, x);
        EXPECT_EQ(5, F.mul(*x, 3));
        // 2 / 4
        x = F.div(2, 4);
        EXPECT_NE(std::nullopt, x);
        EXPECT_EQ(2, F.mul(*x, 4));
        // (prime - 1) / (prime - 2)
        x = F.div(prime - 1, prime - 2);
        EXPECT_NE(std::nullopt, x);
        EXPECT_EQ(prime - 1, F.mul(*x, prime - 2));
        // (prime - 2) / (prime - 1)
        x = F.div(prime - 2, prime - 1);
        EXPECT_NE(std::nullopt, x);
        EXPECT_EQ(prime - 2, F.mul(*x, prime - 1));
        // div by zero
        x = F.div(1, 0);
        EXPECT_EQ(std::nullopt, x);
    }
}

TEST(fp1, pow) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(*F.pow(2, 3), 8);
        EXPECT_EQ(*F.pow(2, 0), 1);
        EXPECT_EQ(*F.pow(0, 5), 0);
        EXPECT_EQ(*F.pow(5, 1), 5);
        std::optional<BigInt> x;
        // 2 ^ (-3)
        x = F.pow(2, -3);
        EXPECT_NE(std::nullopt, x);
        EXPECT_EQ(F.mod_reduce(*x * *F.pow(2, 3)), 1);
        // (-2) ^ 3
        x = F.pow(-2, 3);
        EXPECT_NE(std::nullopt, x);
        EXPECT_EQ(x, F.pow(prime - 2, 3)); // (-2) ^ 3 ≡ (p - 2) ^ 3
        // (-2) ^ (-3)
        x = F.pow(-2, -3);
        EXPECT_NE(std::nullopt, x);
        EXPECT_EQ(F.mod_reduce(*x * *F.pow(prime - 2, 3)), 1);
        // 0 ^ (-1)
        x = F.pow(0, -1);
        EXPECT_EQ(std::nullopt, x);
        // 1 ^ 0
        x = F.pow(1, 0);
        EXPECT_EQ(1, x);
        // 1 ^ (-1)
        x = F.pow(1, -1);
        EXPECT_EQ(1, x);
    }
}

TEST(fp1, square) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(F.square(0), 0);
        EXPECT_EQ(F.square(1), 1);
        EXPECT_EQ(F.square(2), 4);
        EXPECT_EQ(F.square(3), 9);
        EXPECT_EQ(F.square(4), 16);
        EXPECT_EQ(F.square(-4), 16);
        EXPECT_EQ(F.square(prime), 0);
        EXPECT_EQ(F.square(prime - 1), 1);
        EXPECT_EQ(F.square(prime - 2), 4);
        EXPECT_EQ(F.square(prime - 3), 9);
        EXPECT_EQ(F.square(prime + 1), 1);
        EXPECT_EQ(F.square(prime + 2), 4);
        EXPECT_EQ(F.square(prime + 3), 9);
    }
}

TEST(fp1, sqrt) {
    init();
    std::optional<BigInt> x;
    for (const auto& prime : PRIMES) {
        Fp1 F(prime);
        // a = 0
        x = F.sqrt(0);
        EXPECT_EQ(0, *x);
        // a = 0 ~ 16
        SCOPED_TRACE("Prime = " + prime.str(10));
        for (int32_t i = 0; i < 17; ++i) {
            x = F.sqrt(i);
            if (std::nullopt == x) {
                continue;
            }
            EXPECT_EQ(i, F.square(*x));
        }
        // a = prime +- offset
        std::vector<BigInt> vec_a = {prime, prime * 2, prime * 2 + 1, prime - 2, prime - 1, prime + 1, prime + 2};
        std::vector<BigInt> vec_x = {     0,        0,             1, prime - 2, prime - 1,         1,         2};
        for (size_t i = 0; i < vec_a.size(); ++i) {
            x = F.sqrt(vec_a[i]);
            if (std::nullopt != x) {
                EXPECT_EQ(vec_x[i], F.square(*x));
            }
        }
    }
    // a = -1
    Fp1 F(PRIME_17);
    x = F.sqrt(-1);
    EXPECT_NE(std::nullopt, x);
    EXPECT_EQ(PRIME_17 - 1, F.square(*x));
}

TEST(fp1, mod_inv) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(std::nullopt, F.mod_inv(0));
        EXPECT_EQ(std::nullopt, F.mod_inv(prime));
        EXPECT_EQ(std::nullopt, F.mod_inv(-prime));
        EXPECT_EQ(F.mul(*F.mod_inv(1), 1), 1);
        EXPECT_EQ(F.mul(*F.mod_inv(-1), -1), 1);
        EXPECT_EQ(F.mul(*F.mod_inv(-2), -2), 1);
        EXPECT_EQ(F.mul(*F.mod_inv(2), 2), 1);
        EXPECT_EQ(F.mul(*F.mod_inv(prime - 1), prime - 1), 1);
        EXPECT_EQ(F.mul(*F.mod_inv(prime + 1), prime + 1), 1);
    }
}

TEST(fp1, neg) {
    init();
    for (const auto& prime : PRIMES) {
        SCOPED_TRACE("Prime = " + prime.str(10));
        Fp1 F(prime);
        EXPECT_EQ(F.neg(0), 0);
        EXPECT_EQ(F.neg(1), prime - 1);
        EXPECT_EQ(F.neg(2), prime - 2);
        EXPECT_EQ(F.neg(-2), 2);
        EXPECT_EQ(F.neg(prime), 0);
        EXPECT_EQ(F.neg(prime - 1), 1);
        EXPECT_EQ(F.neg(prime - 2), 2);
        EXPECT_EQ(F.neg(prime + 1), prime - 1);
        EXPECT_EQ(F.neg(prime + 2), prime - 2);
    }
}

TEST(fp1, has_sqrt) {
    Fp1 F(PRIME_17);
    std::vector<BigInt> with_sqrt = {1, 2, 4, 8, 9, 13, 15, 16};
    std::vector<BigInt> no_sqrt   = {3, 5, 6, 7, 10, 11, 12, 14};
    for (const auto& a : with_sqrt) {
        ASSERT_TRUE(F.has_sqrt(a)) << "Should have sqrt: " << a;
    }
    for (const auto& a : no_sqrt) {
        ASSERT_FALSE(F.has_sqrt(a)) << "Should not have sqrt: " << a;
    }
}

TEST(fp1, peformance_mod_reduce) {
    Fp1 F(ec::babyjub::prime());
    auto a = F.mul(ec::babyjub::prime(), 2);
    PERFORMANCE_TEST(TEN_MILLION, {
        auto x = F.mod_reduce(a);
    })
}

TEST(fp1, peformance_add) {
    Fp1 F(ec::babyjub::prime());
    auto a = *F.div(ec::babyjub::prime(), 2);
    auto b = *F.div(ec::babyjub::prime(), 3);
    PERFORMANCE_TEST(TEN_MILLION, {
        auto x = F.add(a, b);
    })
}

TEST(fp1, peformance_sub) {
    Fp1 F(ec::babyjub::prime());
    auto a = *F.div(ec::babyjub::prime(), 2);
    auto b = *F.div(ec::babyjub::prime(), 3);
    PERFORMANCE_TEST(ONE_MILLION, {
        auto x = F.sub(a, b);
    })
}

TEST(fp1, peformance_mul) {
    Fp1 F(ec::babyjub::prime());
    auto a = *F.div(ec::babyjub::prime(), 2);
    auto b = *F.div(ec::babyjub::prime(), 3);
    PERFORMANCE_TEST(ONE_MILLION, {
        auto x = F.mul(a, b);
    })
}

TEST(fp1, peformance_div) {
    Fp1 F(ec::babyjub::prime());
    std::optional<BigInt> x;
    auto a = ec::babyjub::prime() - 1;
    auto b = ec::babyjub::prime() - 2;
    PERFORMANCE_TEST(ONE_MILLION, {
        x = F.div(a, b);
    })
    EXPECT_NE(std::nullopt, x);
}

TEST(fp1, peformance_pow) {
    Fp1 F(ec::babyjub::prime());
    std::optional<BigInt> x;
    auto a = *F.div(ec::babyjub::prime(), 2);
    auto b = *F.div(ec::babyjub::prime(), 3);
    PERFORMANCE_TEST(ONE_MILLION, {
        x = F.pow(a, b);
    })
    EXPECT_NE(std::nullopt, x);
}

TEST(fp1, peformance_square) {
    Fp1 F(ec::babyjub::prime());
    auto a = *F.div(ec::babyjub::prime(), 2);
    PERFORMANCE_TEST(TEN_MILLION, {
        auto x = F.square(a);
    })
}

TEST(fp1, peformance_sqrt) {
    Fp1 F(ec::babyjub::prime());
    std::optional<BigInt> x;
    auto a = *F.div(ec::babyjub::prime(), 2);
    PERFORMANCE_TEST(TEN_MILLION, {
        x = F.sqrt(a);
    })
    EXPECT_NE(std::nullopt, x);
}

TEST(fp1, peformance_mod_inv) {
    Fp1 F(ec::babyjub::prime());
    std::optional<BigInt> x;
    auto a = ec::babyjub::prime() - 1;
    PERFORMANCE_TEST(ONE_MILLION, {
        x = F.mod_inv(a);
    })
    EXPECT_NE(std::nullopt, x);
}

TEST(fp1, peformance_has_sqrt) {
    Fp1 F(ec::babyjub::prime());
    auto a = *F.div(ec::babyjub::prime(), 2);
    PERFORMANCE_TEST(TEN_MILLION, {
        auto r = F.has_sqrt(a);
    })
}

TEST(fp1, peformance_neg) {
    Fp1 F(ec::babyjub::prime());
    auto a = *F.div(ec::babyjub::prime(), 2);
    PERFORMANCE_TEST(TEN_MILLION, {
        auto x = F.neg(a);
    })
}

} // namespace iden3math
