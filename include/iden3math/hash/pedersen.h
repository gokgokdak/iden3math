#pragma once

#include <iden3math/macro.h>
#include <iden3math/typedef.h>

namespace iden3math::hash {

API void pedersen(const ByteVec1D& preimage, ByteVec1D& digest);

} // namespace iden3math::hash
