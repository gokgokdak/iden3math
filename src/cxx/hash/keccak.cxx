#include <iden3math/hash/keccak.h>
#include <iden3math/serialize.h>
#include <cstring>

namespace iden3math::hash {

/* Keccak 256 pre-image input memory layout
 *
 * Text
 *   String: "This is a message"
 *   Memory: (Low Addr)['T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'm', 'e', 's', 's', 'a', 'g', 'e'](High Addr)
 *
 * Number
 *   BigInt: 0xffeeddccbbaa99887766554433221100
 *   Memory: (Low Addr)[0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00](High Addr)
 */

// State Matrix:
// [  a[0, 0]  a[0, 1]  a[0, 2]  a[0, 3]  a[0, 4]  ]
// [  a[1, 0]  a[1, 1]  a[1, 2]  a[1, 3]  a[1, 4]  ]
// [  a[2, 0]  a[2, 1]  a[2, 2]  a[2, 3]  a[2, 4]  ]
// [  a[3, 0]  a[3, 1]  a[3, 2]  a[3, 3]  a[3, 4]  ]
// [  a[4, 0]  a[4, 1]  a[4, 2]  a[4, 3]  a[4, 4]  ]

// block[0]  = a[0, 0], block[1]  = a[1, 0], block[2]  = a[2, 0], block[3]  = a[3, 0], block[4]  = a[4, 0]
// block[5]  = a[0, 1], block[6]  = a[1, 1], block[7]  = a[2, 1], block[8]  = a[3, 1], block[9]  = a[4, 1]
// block[10] = a[0, 2], block[11] = a[1, 2], block[12] = a[2, 2], block[13] = a[3, 2], block[14] = a[4, 2]
// block[15] = a[0, 3], block[16] = a[1, 3], block[17] = a[2, 3], block[18] = a[3, 3], block[19] = a[4, 3]
// block[20] = a[0, 4], block[21] = a[1, 4], block[22] = a[2, 4], block[23] = a[3, 4], block[24] = a[4, 4]

static constexpr uint64_t ROUND_CONSTANTS_F1600[24] = {
    0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
    0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
    0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
    0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
    0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
    0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
    0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
};

uint64_t rotate_left(uint64_t r, int n) {
    return (r << n) | (r >> (64 - n));
}

void f1600(uint64_t* block) {
    uint64_t C[5];
    uint64_t tmp1, tmp2;

    for (const uint64_t constant : ROUND_CONSTANTS_F1600) {
        // THETA
        C[0] = block[0]  ^ block[1]  ^ block[2]  ^ block[3]  ^ block[4];
        C[1] = block[5]  ^ block[6]  ^ block[7]  ^ block[8]  ^ block[9];
        C[2] = block[10] ^ block[11] ^ block[12] ^ block[13] ^ block[14];
        C[3] = block[15] ^ block[16] ^ block[17] ^ block[18] ^ block[19];
        C[4] = block[20] ^ block[21] ^ block[22] ^ block[23] ^ block[24];
        block[0]  ^= C[4] ^ rotate_left(C[1], 1); block[1]  ^= C[4] ^ rotate_left(C[1], 1); block[2]  ^= C[4] ^ rotate_left(C[1], 1); block[3]  ^= C[4] ^ rotate_left(C[1], 1); block[4]  ^= C[4] ^ rotate_left(C[1], 1);
        block[5]  ^= C[0] ^ rotate_left(C[2], 1); block[6]  ^= C[0] ^ rotate_left(C[2], 1); block[7]  ^= C[0] ^ rotate_left(C[2], 1); block[8]  ^= C[0] ^ rotate_left(C[2], 1); block[9]  ^= C[0] ^ rotate_left(C[2], 1);
        block[10] ^= C[1] ^ rotate_left(C[3], 1); block[11] ^= C[1] ^ rotate_left(C[3], 1); block[12] ^= C[1] ^ rotate_left(C[3], 1); block[13] ^= C[1] ^ rotate_left(C[3], 1); block[14] ^= C[1] ^ rotate_left(C[3], 1);
        block[15] ^= C[2] ^ rotate_left(C[4], 1); block[16] ^= C[2] ^ rotate_left(C[4], 1); block[17] ^= C[2] ^ rotate_left(C[4], 1); block[18] ^= C[2] ^ rotate_left(C[4], 1); block[19] ^= C[2] ^ rotate_left(C[4], 1);
        block[20] ^= C[3] ^ rotate_left(C[0], 1); block[21] ^= C[3] ^ rotate_left(C[0], 1); block[22] ^= C[3] ^ rotate_left(C[0], 1); block[23] ^= C[3] ^ rotate_left(C[0], 1); block[24] ^= C[3] ^ rotate_left(C[0], 1);

        // RHOPI
        tmp1 = block[8];
        block[8]  = rotate_left(block[1] , 36);
        block[1]  = rotate_left(block[15], 28);
        block[15] = rotate_left(block[18], 21);
        block[18] = rotate_left(block[13], 15);
        block[13] = rotate_left(block[7] , 10);
        block[7]  = rotate_left(block[11], 6 );
        block[11] = rotate_left(block[2] , 3 );
        block[2]  = rotate_left(block[5] , 1 );
        block[5]  = rotate_left(block[6] , 44);
        block[6]  = rotate_left(block[21], 20);
        block[21] = rotate_left(block[14], 61);
        block[14] = rotate_left(block[22], 39);
        block[22] = rotate_left(block[4] , 18);
        block[4]  = rotate_left(block[10], 62);
        block[10] = rotate_left(block[12], 43);
        block[12] = rotate_left(block[17], 25);
        block[17] = rotate_left(block[23], 8 );
        block[23] = rotate_left(block[19], 56);
        block[19] = rotate_left(block[3] , 41);
        block[3]  = rotate_left(block[20], 27);
        block[20] = rotate_left(block[24], 14);
        block[24] = rotate_left(block[9] , 2 );
        block[9]  = rotate_left(block[16], 55);
        block[16] = rotate_left(tmp1     , 45);

        // Rho Pi
        tmp1 = block[0];
        tmp2 = block[5];
        block[0]  ^= (~block[5]  & block[10]);
        block[5]  ^= (~block[10] & block[15]);
        block[10] ^= (~block[15] & block[20]);
        block[15] ^= (~block[20] & tmp1     );
        block[20] ^= (~tmp1      & tmp2     );
        tmp1 = block[1];
        tmp2 = block[6];
        block[1]  ^= (~block[6]  & block[11]);
        block[6]  ^= (~block[11] & block[16]);
        block[11] ^= (~block[16] & block[21]);
        block[16] ^= (~block[21] & tmp1     );
        block[21] ^= (~tmp1      & tmp2     );
        tmp1 = block[2];
        tmp2 = block[7];
        block[2]  ^= (~block[7]  & block[12]);
        block[7]  ^= (~block[12] & block[17]);
        block[12] ^= (~block[17] & block[22]);
        block[17] ^= (~block[22] & tmp1     );
        block[22] ^= (~tmp1      & tmp2     );
        tmp1 = block[3];
        tmp2 = block[8];
        block[3]  ^= (~block[8]  & block[13]);
        block[8]  ^= (~block[13] & block[18]);
        block[13] ^= (~block[18] & block[23]);
        block[18] ^= (~block[23] & tmp1     );
        block[23] ^= (~tmp1      & tmp2     );
        tmp1 = block[4];
        tmp2 = block[9];
        block[4]  ^= (~block[9]  & block[14]);
        block[9]  ^= (~block[14] & block[19]);
        block[14] ^= (~block[19] & block[24]);
        block[19] ^= (~block[24] & tmp1     );
        block[24] ^= (~tmp1      & tmp2     );

        block[0] ^= constant;
    }
}

void keccak256(const ByteVec1D& bytes, ByteVec1D& digest) {
    uint32_t block_pos[25]  = {0, 5, 10, 15, 20, 1, 6, 11, 16, 21, 2, 7, 12, 17, 22, 3, 8, 13, 18, 23, 4, 9, 14, 19, 24};
    uint32_t num_of_seg_136 = bytes.size() / 136;       // Number of segments that has exactly 136 bytes
    uint32_t rest_data_size = bytes.size() % 136;       // Number of remaining bytes that is less than 136
    uint32_t blks_per_seg   = 136 / sizeof(uint64_t);   // Number of 64-bit blocks in a segment
    uint32_t feed           = 0;
    uint64_t state[25]      = {};

    // Absorb, XOR the whole 136-byte segments, if it has
    for (uint32_t i = 0; i < num_of_seg_136; ++i) {
        for (size_t blk = 0; blk < blks_per_seg; ++blk) {
            state[block_pos[blk]] ^= *reinterpret_cast<const uint64_t*>(bytes.data() + feed);
            feed += sizeof(uint64_t);
        }
        f1600(state);
    }

    // Absorb, XOR the last segment
    size_t blk = 0;
    uint64_t pad_start_block = 0;
    // XOR 8-byte blocks of rest data
    uint32_t rest_blocks = rest_data_size / sizeof(uint64_t);
    for (; blk < rest_blocks; ++blk) {
        state[block_pos[blk]] ^= *reinterpret_cast<const uint64_t*>(bytes.data() + feed);
        feed += sizeof(uint64_t);
    }
    // Copy rest bytes
    size_t rest_bytes = rest_data_size % sizeof(uint64_t);
    std::memcpy(&pad_start_block, bytes.data() + feed, rest_bytes);
    feed += rest_bytes;
    // Pad10*1
    pad_start_block |= static_cast<uint64_t>(0x01) << rest_bytes * 8;
    if (blk == blks_per_seg - 1) {
        state[block_pos[blk]] ^= pad_start_block | 0x8000000000000000;
    } else {
        state[block_pos[blk]] ^= pad_start_block;
    }
    ++blk;
    for (; blk < blks_per_seg; ++blk) {
        if (blk == blks_per_seg - 1) {
            state[block_pos[blk]] ^= 0x8000000000000000;
        } else {
            state[block_pos[blk]] ^= 0x00;
        }
    }
    // Permutation
    f1600(state);

    // Squeeze out the digest
    digest.clear();
    digest.resize(32);
    digest.reserve(32);
    std::memcpy(&digest[0 * sizeof (uint64_t)], &state[0], sizeof(uint64_t));
    std::memcpy(&digest[1 * sizeof (uint64_t)], &state[5], sizeof(uint64_t));
    std::memcpy(&digest[2 * sizeof (uint64_t)], &state[10], sizeof(uint64_t));
    std::memcpy(&digest[3 * sizeof (uint64_t)], &state[15], sizeof(uint64_t));
}

void keccak256(const std::string& text, ByteVec1D& digest) {
    ByteVec1D bytes;
    for (const char& c : text) {
        bytes.push_back(static_cast<uint8_t>(c));
    }
    keccak256(bytes, digest);
}

void keccak256(const BigInt& number, ByteVec1D& digest) {
    ByteVec1D bytes;
    auto hex = number.str(16);
    std::string error;
    if (!serialize::hexstr_to_bytes(hex, bytes, error)) {
        throw std::runtime_error("Failed to convert BigInt to bytes: " + error);
    }
    keccak256(bytes, digest);
}

} // namespace iden3math::hash
