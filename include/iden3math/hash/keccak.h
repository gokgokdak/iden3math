#pragma once

#include <iden3math/bigint.h>
#include <iden3math/macro.h>

namespace iden3math::hash {

// Parameter `bytes` and `digest` are allowed to be the same vector
API void keccak256(const ByteVec1D& bytes, ByteVec1D& digest);

API void keccak256(const std::string& text, ByteVec1D& digest);

API void keccak256(const BigInt& number, ByteVec1D& digest);

} // namespace iden3math::hash
