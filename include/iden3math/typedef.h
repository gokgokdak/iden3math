#pragma once

#include <cstdint>
#include <vector>

namespace iden3math {

template <typename T>
using enable_if_int_t = std::enable_if_t<std::is_integral_v<T>, int>;

template <typename T>
using enable_if_int_signed_t = std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, int>;

template <typename T>
using enable_if_int_unsigned_t = std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, int>;

using Bit = bool;
using Byte = uint8_t;
using ByteVec1D = std::vector<Byte>;
using ByteVec2D = std::vector<ByteVec1D>;

typedef enum Endian {
    LE = 0,
    BE = 1,
} Endian;

} // namespace iden3math
