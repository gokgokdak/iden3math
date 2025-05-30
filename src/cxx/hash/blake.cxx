#include <iden3math/hash/blake.h>
#include <cstring>

namespace iden3math::hash {

typedef struct {
    uint32_t h[8];
    uint32_t s[4];
    uint32_t t[2];
    int32_t buflen;
    int32_t nullt;
    Byte buf[64];
} State256;

static const Byte SIGMA[][16] = {
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15},
    {14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3},
    {11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4},
    { 7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8},
    { 9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13},
    { 2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9},
    {12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11},
    {13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10},
    { 6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5},
    {10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13 , 0},
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15},
    {14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3},
    {11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4},
    { 7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8},
    { 9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13},
    { 2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9},
};

static const Byte PADDING[129] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint32_t U256[16] = {
    0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344,
    0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89,
    0x452821e6, 0x38d01377, 0xbe5466cf, 0x34e90c6c,
    0xc0ac29b7, 0xc97c50dd, 0x3f84d5b5, 0xb5470917
};

#define U8TO32_BIG(p) (                                         \
    ((uint32_t)((p)[0]) << 24) | ((uint32_t)((p)[1]) << 16) |   \
    ((uint32_t)((p)[2]) <<  8) | ((uint32_t)((p)[3])      )     \
)

#define U32TO8_BIG(p, v)				                    \
    (p)[0] = (Byte)((v) >> 24); (p)[1] = (Byte)((v) >> 16); \
    (p)[2] = (Byte)((v) >>  8); (p)[3] = (Byte)((v)      );

#define ROT(x,n) (((x)<<(32-n))|( (x)>>(n)))

#define G(a,b,c,d,e)                                        \
    v[a] += (m[SIGMA[i][e]] ^ U256[SIGMA[i][e+1]]) + v[b];  \
    v[d] = ROT( v[d] ^ v[a],16);                            \
    v[c] += v[d];                                           \
    v[b] = ROT( v[b] ^ v[c],12);                            \
    v[a] += (m[SIGMA[i][e+1]] ^ U256[SIGMA[i][e]])+v[b];    \
    v[d] = ROT( v[d] ^ v[a], 8);                            \
    v[c] += v[d];                                           \
    v[b] = ROT( v[b] ^ v[c], 7);

void blake256_compress(State256 *S, const Byte* block) {
    uint32_t v[16];
    uint32_t m[16];

    for (size_t i = 0; i < 16; ++i) {
        m[i] = U8TO32_BIG(block + i * 4);
    }
    for (size_t i = 0; i < 8; ++i) {
        v[i] = S->h[i];
    }
    v[ 8] = S->s[0] ^ U256[0];
    v[ 9] = S->s[1] ^ U256[1];
    v[10] = S->s[2] ^ U256[2];
    v[11] = S->s[3] ^ U256[3];
    v[12] = U256[4];
    v[13] = U256[5];
    v[14] = U256[6];
    v[15] = U256[7];

    if ( !S->nullt ) { // Don't xor t when the block is only padding
        v[12] ^= S->t[0];
        v[13] ^= S->t[0];
        v[14] ^= S->t[1];
        v[15] ^= S->t[1];
    }
    for (size_t i = 0; i < 14; ++i) {
        // Column step
        G(0, 4,  8, 12, 0);
        G(1, 5,  9, 13, 2);
        G(2, 6, 10, 14, 4);
        G(3, 7, 11, 15, 6);
        // Diagonal step
        G(0, 5, 10, 15,  8);
        G(1, 6, 11, 12, 10);
        G(2, 7,  8, 13, 12);
        G(3, 4,  9, 14, 14);
    }
    for (size_t i = 0; i < 16; ++i) {
        S->h[i % 8] ^= v[i];
    }
    for (size_t i = 0; i < 8 ; ++i) {
        S->h[i] ^= S->s[i % 4];
    }
}

void blake256_init(State256* S) {
    S->h[0] = 0x6a09e667;
    S->h[1] = 0xbb67ae85;
    S->h[2] = 0x3c6ef372;
    S->h[3] = 0xa54ff53a;
    S->h[4] = 0x510e527f;
    S->h[5] = 0x9b05688c;
    S->h[6] = 0x1f83d9ab;
    S->h[7] = 0x5be0cd19;
    memset(S->s, 0x00, sizeof(S->s));
    memset(S->t, 0x00, sizeof(S->t));
    S->buflen = 0;
    S->nullt = 0;
    memset(S->buf, 0x00, sizeof(S->buf));
}

void blake256_update(State256& S, const Byte* data, size_t len) {
    auto left = S.buflen;
    auto fill = 64 - left;

    if (left > 0 && len >= fill) {
        memcpy(S.buf + left, data, fill);
        S.t[0] += 512;
        if ( S.t[0] == 0 ) {
            ++S.t[1];
        }
        blake256_compress(&S, S.buf);
        data += fill;
        len -= fill;
        left = 0;
    }
    while (len >= 64) {
        S.t[0] += 512;
        if ( S.t[0] == 0 ) {
            ++S.t[1];
        }
        blake256_compress(&S, data);
        data += 64;
        len -= 64;
    }
    if (len > 0) {
        memcpy(S.buf + left, data, len);
        S.buflen = left + len;
    } else {
        S.buflen = 0;
    }
}

void Blake256Final(State256 S, Byte* out) {
    Byte msg_len[8];
    Byte zo = 0x01;
    Byte oo = 0x81;
    uint32_t lo = S.t[0] + (S.buflen << 3);
    uint32_t hi = S.t[1];

    if (lo < S.buflen << 3) { // Support for hashing more than 2^32 bits
        ++hi;
    }
    U32TO8_BIG(msg_len + 0, hi);
    U32TO8_BIG(msg_len + 4, lo);

    if (S.buflen == 55) { // One padding byte
        S.t[0] -= 8;
        blake256_update(S, &oo, 1);
    } else {
        if (S.buflen < 55) { // enough space to fill the block
            if (!S.buflen) {
                S.nullt = 1;
            }
            S.t[0] -= 440 - (S.buflen << 3);
            blake256_update(S, PADDING, 55 - S.buflen);
        } else { // Need 2 compressions
            S.t[0] -= 512 - (S.buflen << 3 );
            blake256_update(S, PADDING, 64 - S.buflen);
            S.t[0] -= 440;
            blake256_update(S, PADDING + 1, 55);
            S.nullt = 1;
        }
        blake256_update(S, &zo, 1);
        S.t[0] -= 8;
    }
    S.t[0] -= 64;
    blake256_update(S, msg_len, 8);
    U32TO8_BIG(out + 0, S.h[0]);
    U32TO8_BIG(out + 4, S.h[1]);
    U32TO8_BIG(out + 8, S.h[2]);
    U32TO8_BIG(out + 12, S.h[3]);
    U32TO8_BIG(out + 16, S.h[4]);
    U32TO8_BIG(out + 20, S.h[5]);
    U32TO8_BIG(out + 24, S.h[6]);
    U32TO8_BIG(out + 28, S.h[7]);
}

void blake256(const ByteVec1D& bytes, ByteVec1D& digest) {
    if (bytes.empty()) {
        digest = {};
        return;
    }
    digest.clear();
    digest.resize(32);
    digest.reserve(32);
    State256 S;
    blake256_init(&S);
    blake256_update(S, bytes.data(), bytes.size());
    Blake256Final(S, digest.data());
}

ByteVec1D blake256(const ByteVec1D& bytes) {
    ByteVec1D digest;
    blake256(bytes, digest);
    return digest;
}

void blake256(const std::string& text, ByteVec1D& digest) {
    if (text.empty()) {
        digest = {};
        return;
    }
    ByteVec1D bytes(text.begin(), text.end());
    blake256(bytes, digest);
}

ByteVec1D blake256(const std::string& text) {
    ByteVec1D digest;
    blake256(text, digest);
    return digest;
}


} // namespace iden3math::hash
