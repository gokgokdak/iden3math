#pragma once

#include <iden3math/bigint.h>
#include <iden3math/macro.h>
#include <iden3math/typedef.h>
#include <optional>

namespace iden3math::random {

// fallback: true if downgrade to std::random_device
API ByteVec1D get_bytes(size_t len, std::optional<std::reference_wrapper<bool>> fallback = std::nullopt);

// r in [1, p - 1]
// fallback: true if downgrade to std::random_device
API BigInt get_integer(const BigInt& p, std::optional<std::reference_wrapper<bool>> fallback = std::nullopt);

} // namespace iden3math::random
