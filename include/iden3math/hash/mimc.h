#pragma once

#include <iden3math/bigint.h>
#include <iden3math/macro.h>

namespace iden3math::hash {

API void mimc_sponge(const ByteVec2D& preimages, size_t outputs, const ByteVec1D& key, ByteVec2D& digests, Endian preimage_endian = BE, Endian key_endian = BE, Endian digest_endian = BE);

} // namespace iden3math::hash
