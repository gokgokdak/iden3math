#include <iden3math/bigint.h>
#include <iden3math/ec/babyjub.h>
#include <iden3math/ec/point.h>
#include <iden3math/hash/blake.h>
#include <iden3math/hash/pedersen.h>
#include <iden3math/serialize.h>
#include <cmath>
#include <mutex>
#include <unordered_map>

namespace iden3math::hash {

size_t WINDOW_SIZE = 4; // 4 bits

size_t WINDOWS_PER_SEG = 50;

size_t BITES_PER_SEG = WINDOW_SIZE * WINDOWS_PER_SEG;

static const std::string GEN_POINT_PREFIX = "PedersenGenerator";

static std::mutex BASE_POINTS_MUTEX;

static std::unordered_map<size_t, ec::Point> BASE_POINTS;

std::string uint_to_32figures_decstr(uint32_t number) {
    BigInt num(number);
    std::string str = num.str(10);
    if (str.size() >= 32) {
        return str;
    }
    return std::string(32 - str.size(), '0') + str;
}

ec::Point get_base_point(uint32_t index) {
    {
        std::lock_guard lock(BASE_POINTS_MUTEX);
        if (BASE_POINTS.end() != BASE_POINTS.find(index)) {
            return BASE_POINTS[index];
        }
    }
    std::optional<ec::Point> p;
    uint32_t try_idx = 0;
    while (std::nullopt == p) {
        auto S = GEN_POINT_PREFIX + "_" + uint_to_32figures_decstr(index) + "_" + uint_to_32figures_decstr(try_idx);
        ByteVec1D digest = blake256(S);
        digest[31] &= 0xbf;  // Set 255th bit to 0 (256th is the signal and 254th is the last possible bit to 1)
        p = ec::babyjub::decompress(digest, LE);
        ++try_idx;
    }
    auto p8 = ec::babyjub::mul_scalar(*p, 8);
    if (!ec::babyjub::in_sub_group(p8)) {
        throw std::runtime_error("Point not in curve");
    }
    std::lock_guard lock(BASE_POINTS_MUTEX);
    BASE_POINTS[index] = p8;
    return p8;
}

void pedersen(const ByteVec1D& preimage, ByteVec1D& digest) {
    auto bits = serialize::bytes_to_bits(preimage);
    auto n_segments = static_cast<uint32_t>(floor((bits.size() - 1) / BITES_PER_SEG) + 1);
    if (0 == bits.size()) {
        n_segments = 0;
    }
    ec::Point acc_p(0, 1);

    for (uint32_t seg = 0; seg < n_segments; ++seg) {
        size_t n_windows;
        if (seg == n_segments - 1) {
            n_windows = static_cast<size_t>(floor((bits.size() - (n_segments - 1) * BITES_PER_SEG - 1) / WINDOW_SIZE) + 1);
        } else {
            n_windows = WINDOWS_PER_SEG;
        }
        BigInt escalar(0);
        BigInt exp(1);
        for (size_t w = 0; w < n_windows; ++w) {
            auto o = seg * BITES_PER_SEG + w * WINDOW_SIZE;
            BigInt acc(1);
            for (size_t b = 0; b < WINDOW_SIZE - 1 && o < bits.size(); ++b) {
                if (bits[o]) {
                    acc += 1 << b;
                }
                ++o;
            }
            if (o < bits.size()) {
                if (bits[o]) {
                    acc = -acc;
                }
                ++o;
            }
            escalar += acc * exp;
            exp <<= WINDOW_SIZE + 1;
        }
        if (escalar < 0) {
            escalar += ec::babyjub::sub_group_order();
        }
        acc_p = ec::babyjub::add(acc_p, ec::babyjub::mul_scalar(get_base_point(seg), escalar));
    }
    digest = ec::babyjub::compress(acc_p, LE);
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
}

} // namespace iden3math::hash
