#include <iden3math/bigint.h>
#include <random>
#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    #include <cstdio>
#elif defined(_WIN32)
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
#endif

namespace iden3math::random {

ByteVec1D get_bytes(size_t len, std::optional<std::reference_wrapper<bool>> fallback) {
    ByteVec1D bytes(len);
    bool succeed = false;
    if (std::nullopt != fallback) {
        fallback->get() = false;
    }
#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    // Use /dev/hwrng or /dev/urandom on Unix-like systems
    auto try_read = [&](const char* path) -> bool {
        FILE* dev = fopen(path, "rb");
        if (nullptr == dev) {
            return false;
        }
        const auto read = fread(bytes.data(), 1, len, dev);
        fclose(dev);
        if (read == len) {
            return true;
        }
        return false;
    };
    succeed = try_read("/dev/hwrng") || try_read("/dev/urandom");
#elif defined(_WIN32)
    // Use Windows CryptoAPI
    succeed = BCRYPT_SUCCESS(BCryptGenRandom(
        nullptr, // Use system-preferred RNG algorithm (e.g., RNGCryptoServiceProvider)
        bytes.data(), bytes.size(),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    ));
#endif
    // Fallback to std::random_device if system-specific methods fail
    if (!succeed) {
        if (std::nullopt != fallback) {
            fallback->get() = true;
        }
        std::random_device rd;
        for (size_t byte = 0; byte < len; ++byte) {
            for (size_t bit = 0; bit < 8; ++bit) {
                bytes[byte] |= ((rd() & 1) << bit);
            }
        }
    }
    return bytes;
}

BigInt get_integer(const BigInt& p, std::optional<std::reference_wrapper<bool>> fallback) {
    BigInt r;
    do {
        r = BigInt(get_bytes(p.bytes_size(), fallback), LE) % p;
    } while (r == 0);
    return r;
}

} // namespace iden3math::random
