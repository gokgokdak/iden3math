#pragma once

#include <iden3math/macro.h>
#include <iden3math/typedef.h>
#include <string>

namespace iden3math::hash {

API void blake256(const ByteVec1D& bytes, ByteVec1D& digest);

API ByteVec1D blake256(const ByteVec1D& bytes);

API void blake256(const std::string& text, ByteVec1D& digest);

API ByteVec1D blake256(const std::string& text);

} // namespace iden3math::hash
