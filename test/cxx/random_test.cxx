#include <iden3math/random.h>
#include <gtest/gtest.h>
#include "helper.h"

namespace iden3math::random {

TEST(random, get_bytes_length) {
    size_t lengths[] = {1, 8, 16, 32, 64, 128, 256};
    for (auto len : lengths) {
        bool fallback;
        auto bytes = get_bytes(len, fallback);
        SCOPED_TRACE("fallback=" + std::string(fallback ? "true" : "false"));
        EXPECT_EQ(len, bytes.size());
    }
}

TEST(random, get_bytes_distribution) {
    // Generate large sample to check distribution
    size_t num_samples = 100000;
    size_t byte_len = 1;
    std::vector<size_t> counts(256, 0);

    for (size_t i = 0; i < num_samples; i++) {
        bool fallback;
        auto bytes = get_bytes(byte_len, fallback);
        SCOPED_TRACE("fallback=" + std::string(fallback ? "true" : "false"));
        counts[bytes[0]]++;
    }

    // Chi-square test for uniformity
    double expected = num_samples / 256.0;
    double chi_square = 0.0;
    for (size_t i = 0; i < 256; i++) {
        double diff = counts[i] - expected;
        chi_square += (diff * diff) / expected;
    }

    // For 255 degrees of freedom, chi-square should be less than 293.25
    // at 95% confidence level
    EXPECT_LT(chi_square, 293.25);
}

TEST(random, get_integer_range) {
    BigInt p("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 16); // secp256k1 prime
    for (int i = 0; i < 1000; i++) {
        BigInt r = get_integer(p);
        EXPECT_GT(r, 0);
        EXPECT_LT(r, p);
    }
}

TEST(random, get_integer_distribution) {
    // Test distribution for small prime to make testing feasible
    BigInt small_p = 17;
    std::vector<size_t> counts(16, 0); // counts for 1 to 16
    size_t num_samples = 16000;

    for (size_t i = 0; i < num_samples; i++) {
        BigInt r = get_integer(small_p);
        counts[r.unsigned_int() - 1]++; // subtract 1 since r is in [1,16]
    }

    // Chi-square test for uniformity
    double expected = num_samples / 16.0;
    double chi_square = 0.0;
    for (size_t i = 0; i < 16; i++) {
        double diff = counts[i] - expected;
        chi_square += (diff * diff) / expected;
    }

    // For 15 degrees of freedom, chi-square should be less than 25.0 
    // at 95% confidence level
    EXPECT_LT(chi_square, 25.0);
}

} // namespace iden3math::random
