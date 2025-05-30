#include <iden3math/bigint.h>
#include <iden3math/hash/keccak.h>
#include <iden3math/hash/mimc.h>
#include <iden3math/prime.h>
#include <iden3math/serialize.h>
#include <mutex>

namespace iden3math::hash {

static constexpr uint32_t ROUNDS = 220;

static constexpr uint32_t EXPONENT = 5;

static const std::string CONSTANT_SEED = "mimcsponge";

static std::vector<BigInt> CONSTANTS;

std::once_flag CONSTANTS_INIT_ONCE_FLAG;

void init() {
    CONSTANTS.reserve(ROUNDS);
    CONSTANTS.emplace_back("00", 16);
    ByteVec1D digest;
    keccak256(CONSTANT_SEED, digest);
    for (size_t i = 1; i < ROUNDS - 1; ++i) {
        if (digest.size() < 32) {
            serialize::pad(digest, 0x00, 32 - digest.size(), true);
        }
        keccak256(digest, digest);
        CONSTANTS.emplace_back(BigInt(digest, BE) % prime::bn254());
    }
    CONSTANTS.emplace_back("00", 16);
}

typedef struct FeistelState {
    BigInt xL_in;
    BigInt xR_in;
    BigInt xL_out;
    BigInt xR_out;
} FeistelState;

void feistel(const BigInt& xL_in, const BigInt& xR_in, const BigInt& k, BigInt& xL_out, BigInt& xR_out) {
    std::call_once(CONSTANTS_INIT_ONCE_FLAG, init);
    BigInt t = 0;
    BigInt c = 0;
    std::vector<BigInt> xL;
    std::vector<BigInt> xR;
    xL.reserve(ROUNDS); xL.resize(ROUNDS);
    xR.reserve(ROUNDS); xR.resize(ROUNDS);

    for (int i = 0; i < ROUNDS; ++i) {
        if (0 == i) {
            t = (k + xL_in) % prime::bn254();
        } else {
            t = (k + xL[i - 1] + CONSTANTS[i]) % prime::bn254();
        }
        BigInt f;
        f = t.pow_mod(EXPONENT, prime::bn254());
        if (i < ROUNDS - 1) {
            xL[i] = ((0 == i ? xR_in : xR[i - 1]) + f) % prime::bn254();
            xR[i] = i==0 ? xL_in : xL[i - 1];
        } else {
            xR_out = (xR[i - 1] + f) % prime::bn254();
            xL_out = xL[i - 1];
        }
    }
}

void mimc_sponge(const ByteVec2D& preimages, size_t outputs, const ByteVec1D& key, ByteVec2D& digests, Endian preimage_endian, Endian key_endian, Endian digest_endian) {
    std::vector<BigInt> in;
    std::vector<BigInt> out;
    for (const auto& data: preimages) {
        in.emplace_back(BigInt(data, preimage_endian) % prime::bn254());
    }
    for (int i = 0; i < outputs; ++i) {
        out.emplace_back(0);
    }
    auto k = BigInt(key, key_endian);
    k = k % prime::bn254();
    const auto inputs = in.size();

    std::vector<FeistelState> states;
    states.reserve(inputs + outputs - 1);
    states.resize(inputs + outputs - 1);

    for (int i = 0; i < inputs; ++i) {
        if (0 == i) {
            states[i].xL_in = in[i];
            states[i].xR_in = 0;
        } else {
            states[i].xL_in = (states[i - 1].xL_out + in[i]) % prime::bn254();
            states[i].xR_in = states[i - 1].xR_out;
        }
        feistel(states[i].xL_in, states[i].xR_in, k, states[i].xL_out, states[i].xR_out);
    }

    out[0] = states[inputs - 1].xL_out;
    for (int i = 0; i < outputs - 1; ++i) {
        states[inputs + i].xL_in = states[inputs + i - 1].xL_out;
        states[inputs + i].xR_in = states[inputs + i - 1].xR_out;
        feistel(states[inputs + i].xL_in, states[inputs + i].xR_in, k, states[inputs + i].xL_out, states[inputs + i].xR_out);
        out[i + 1] = states[inputs + i].xL_out;
    }

    digests.clear();
    for (const auto &o : out) {
        auto hex = o.str(16);
        if (hex.size() > 64) {
            throw std::runtime_error("Hex string longer than 64");
        }
        auto bytes = *serialize::hexstr_to_bytes(hex);
        if (digest_endian != BE) {
            bytes = serialize::swap_endian(bytes);
        }
        serialize::pad(bytes, 0x00, 32 - bytes.size(), digest_endian == BE);
        digests.emplace_back(bytes);
    }
}

} // namespace iden3math::hash
