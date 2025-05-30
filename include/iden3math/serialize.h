#pragma once

#include <iden3math/bitvec.h>
#include <iden3math/macro.h>
#include <iden3math/typedef.h>
#include <optional>
#include <string>

namespace iden3math::serialize {

API void lower(std::string& str);

API ByteVec1D swap_endian(const ByteVec1D& bytes);

API bool hexstr_to_bytes(const std::string& hex, ByteVec1D& bytes, std::optional<std::reference_wrapper<std::string>> error = std::nullopt);

API std::optional<ByteVec1D> hexstr_to_bytes(const std::string& hex, std::optional<std::reference_wrapper<std::string>> error = std::nullopt);

API std::string bytes_to_hexstr(const ByteVec1D& bytes, bool upper = false);

/**
 * Pad continuous bytes to vector
 * @param   bytes   Byte array to pad
 * @param   value   0x00 ~ 0xff
 * @param   count   How many bytes to pad
 * @param   low     Padding start from low or high address
 *                  true : (Low Addr){pad, pad, 0xff, 0xff}(High Addr) -> 0xffff[pad][pad]
 *                  false: (Low Addr){0xff, 0xff, pad, pad}(High Addr) -> 0x[pad][pad]ffff
 */
API void pad(ByteVec1D& bytes, Byte value, uint32_t count, bool low);

API BitVec1D bytes_to_bits(const ByteVec1D& bytes);

/**
 * {{0x11, 0x11}, {0x22, 0x22}, {0x33, 0x33}} -> {0x11, 0x11, 0x22, 0x22, 0x33, 0x33}
 */
API ByteVec1D concat(const ByteVec2D& bytes);

} // namespace iden3math::serialize

