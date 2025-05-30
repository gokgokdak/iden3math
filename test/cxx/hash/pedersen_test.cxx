#include <iden3math/hash/pedersen.h>
#include <iden3math/serialize.h>
#include <gmpxx.h>
#include <gtest/gtest.h>
#include "../helper.h"

namespace iden3math::hash {

TEST(pedersen, empty_input) {
    ByteVec1D preimage;
    auto digest = *serialize::hexstr_to_bytes("01");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, single_byte_input) {
    ByteVec1D preimage = {0x01};
    auto digest = *serialize::hexstr_to_bytes("75c28cc0b8c45fa951bd48ffeb096e3373dac173ee78fc7b58e9ce8dc193b01d");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, two_byte_input) {
    ByteVec1D preimage = {0x01, 0x01};
    auto digest = *serialize::hexstr_to_bytes("70db4185b286d4f8cbc8b7bc8b72e312c3b8eb508d3d61e4cf5e8d59d5a81ba6");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, exactly_one_segment) {
    // One segment = WINDOW_SIZE * WINDOWS_PER_SEG = 4 * 50 = 200 bits = 25 bytes
    ByteVec1D preimage(25, 0xaa);
    auto digest = *serialize::hexstr_to_bytes("52d57e2313839d26950b857c0ee3e9b938cfd928a066ff30c301bf77cc705c06");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, one_segment_plus_one_bit) {
    // 201 bits = 26 bytes (last byte only uses 1 bit)
    ByteVec1D preimage(25, 0xaa); preimage.push_back(0x01);
    auto digest = *serialize::hexstr_to_bytes("b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, common_32_bytes_input) {
    auto preimage = *serialize::hexstr_to_bytes("b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3");
    auto digest = *serialize::hexstr_to_bytes("5e749cf88b25a21a292a3116c8e068af236ac370c51fe8ab96291c68392392a2");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, common_32_bytes_input_0x00_prefix_suffix) {
    auto preimage = *serialize::hexstr_to_bytes("00ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f7632880800");
    auto digest = *serialize::hexstr_to_bytes("f3d0d29c8ca8cb52845f2b02e2dd61b494dd40ba19cfde518d8b1161a6260c29");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, common_64_bytes_input) {
    auto preimage = *serialize::hexstr_to_bytes("b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3");
    auto digest = *serialize::hexstr_to_bytes("de1a2202abf4c240d871b13f547011c7fca48e258d8c0ae82745e5ceb9e1b522");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, common_64_bytes_input_0x00_prefix_suffix) {
    auto preimage = *serialize::hexstr_to_bytes("00ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f7632880800");
    auto digest = *serialize::hexstr_to_bytes("b634ccf13fdba7b9c2666167869686910e6035f37bb31ba37e7df94e20f8801a");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, exactly_two_segments) {
    // Two segments = 400 bits = 50 bytes
    ByteVec1D preimage(50, 0xaa);
    auto digest = *serialize::hexstr_to_bytes("09552440bd1c1bab3003021b564fddcab835ed6248f157d81c3df7581452531c");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, exactly_two_segments_plus_one_bit) {
    // Two segments = 400 bits = 50 bytes
    ByteVec1D preimage(50, 0xaa); preimage.push_back(0x01);
    auto digest = *serialize::hexstr_to_bytes("cbcc6877d2bc7dab56621533f4ce2a5b91be9b842b2ae561fa7546251cfe4e2e");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, large_input_2000_bytes) {
    ByteVec1D preimage(1000, 0xaa);
    ByteVec1D seg(1000, 0xcd);
    preimage.insert(preimage.end(), seg.begin(), seg.end());
    auto digest = *serialize::hexstr_to_bytes("1bf524b6681272083e9014f73ec4093399270d1ea4aeae96c623ec7e34f2a48d");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, all_max_value_bytes) {
    ByteVec1D preimage(100, 0xff);
    auto digest = *serialize::hexstr_to_bytes("47522d13124d066e751a0254ce3a439db81263c0b543376cd27d0bb37e658a0d");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, digest_not_cleared_and_call_hash_func_again) {
    ByteVec1D preimage(100, 0xff);
    auto digest = *serialize::hexstr_to_bytes("47522d13124d066e751a0254ce3a439db81263c0b543376cd27d0bb37e658a0d");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    ByteVec1D output;
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);

    preimage = ByteVec1D(50, 0xaa); preimage.push_back(0x01);
    digest = *serialize::hexstr_to_bytes("cbcc6877d2bc7dab56621533f4ce2a5b91be9b842b2ae561fa7546251cfe4e2e");
    serialize::pad(digest, 0x00, 32 - digest.size(), false);
    pedersen(preimage, output);
    EXPECT_EQ(digest, output);
}

TEST(pedersen, performance_call_64_bytes_input) {
    ByteVec1D input(64, 0xaa);
    ByteVec1D digest;
    PERFORMANCE_TEST(ONE_THOUSAND, {
        pedersen(input, digest);
    })
}

} // namespace iden3math::hash
