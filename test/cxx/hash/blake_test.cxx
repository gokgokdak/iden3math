#include <iden3math/hash/blake.h>
#include <iden3math/serialize.h>
#include <gmpxx.h>
#include <gtest/gtest.h>
#include "../helper.h"

namespace iden3math::hash {

TEST(blake256, empty_input) {
    ByteVec1D digest;
    blake256(ByteVec1D(), digest);
    EXPECT_TRUE(digest.empty());
    blake256("", digest);
    EXPECT_TRUE(digest.empty());
}

TEST(blake256, single_byte_0x00) {
    std::string expected = "0ce8d4ef4dd7cd8d62dfded9d4edb0a774ae6a41929a74da23109e8f11139c87";
    ByteVec1D input = {0x00};
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, single_byte_0xaa) {
    std::string expected = "2057dc48805e7e0a1140f2a80e72d58dd9607dfeffcb0159b98658fc15322c1e";
    ByteVec1D input = {0xaa};
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, single_byte_0xff) {
    std::string expected = "63bf0367c7edf0a5285db26daaeb90629434ce05e7f385de1db83ea3c70bc4cf";
    ByteVec1D input = {0xff};
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_size_minus_one_val_0x00) {  // 55 bytes (block size - 1 for padding byte)
    std::string expected = "dc980544f4181cc43505318e317cdfd4334dab81ae035a28818308867ce23060";
    ByteVec1D input(55, 0x00);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_size_minus_one_val_0xaa) {
    std::string expected = "2cd6a7d2bc557e771cc7b482f987cb18ffd5d2d19cf444ff03f1fcc736da44a4";
    ByteVec1D input(55, 0xaa);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_size_minus_one_val_0xff) {
    std::string expected = "d806c129c0a95654d746419667a9f0878da9cc5d55d77e3e22df7c1b12176010";
    ByteVec1D input(55, 0xff);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_one_full_val_0x00) {  // 64 bytes
    std::string expected = "6d994042954f8dc5633626cd50b2bc66d733a313d67fd9702c5a8149a8028c98";
    ByteVec1D input(64, 0x00);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_one_full_val_0xaa) {
    std::string expected = "00bb5fa2cd91c47bb8bfaf8f8489a2603d7b7d7291894d476d4858961436ae9a";
    ByteVec1D input(64, 0xaa);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_one_full_val_0xff) {
    std::string expected = "80a0ace8b131870da8de11bca85a811f44ece342c57cb8cd5567d2a33685b5be";
    ByteVec1D input(64, 0xff);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_one_full_and_36_bytes_val_0x00) {  // 100 bytes (1.5625 blocks)
    std::string expected = "db10fa7d8a13c4bb74729474485366132da7e221ec651f57a3b7fc3258af9696";
    ByteVec1D input(100, 0x00);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_one_full_and_36_bytes_val_0xaa) {
    std::string expected = "cf274ed07d6e716f979a238151bb9d6c6a67376e5b736046880a30819ee3d3fe";
    ByteVec1D input(100, 0xaa);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_one_full_and_36_bytes_val_0xff) {
    std::string expected = "c9dda01aa7cd6e0c5401ac0d3c4a30abab4c229d7039714be52db6257088c7ae";
    ByteVec1D input(100, 0xff);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_two_full_val_0x00) {  // 128 bytes
    std::string expected = "4c8ed99ae2cfdd5bdaba9f19848fcd98b4c60e122096a47ea565c410a1d567ce";
    ByteVec1D input(128, 0x00);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_two_full_val_0xaa) {
    std::string expected = "5320a6e09347384b05bbe61b5d43bb2055584a56c1f753a9ac93cecff0a271c8";
    ByteVec1D input(128, 0xaa);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, block_two_full_val_0xff) {
    std::string expected = "9f0edf629f1045114aedcbe094093e4f4e7a4b24ca9794219a35499a4969f1d9";
    ByteVec1D input(128, 0xff);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, long_message_1000_bytes_val_0x00) {  // 1000 bytes to test multiple block processing and length overflow
    std::string expected = "e63f5343fc28b480dd8e9586f5cc9b11827e9b317d7a4086326e671b68a5efee";
    ByteVec1D input(1000, 0x00);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, long_message_1000_bytes_val_0xaa) {
    std::string expected = "a821641b802939d182455e571294313674e36877ab362c589728ae182458c513";
    ByteVec1D input(1000, 0xaa);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, long_message_1000_bytes_val_0xff) {
    std::string expected = "13252f0a129e6c30d474dcbde5290ece336a82233ca7fd95e2c3b664fbf68384";
    ByteVec1D input(1000, 0xff);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, text_input) {
    ByteVec1D input0 = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    std::string input1 = "0123456789";
    ByteVec1D digest0;
    ByteVec1D digest1;
    blake256(input0, digest0);
    blake256(input1, digest1);
    EXPECT_EQ(serialize::bytes_to_hexstr(digest0), serialize::bytes_to_hexstr(digest1));
}

TEST(blake256, digest_not_cleared_and_call_hash_func_again) {
    // First call, leave a digest in the container
    std::string expected = "a821641b802939d182455e571294313674e36877ab362c589728ae182458c513";
    ByteVec1D input(1000, 0xaa);
    ByteVec1D digest;
    blake256(input, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
    
    // Second call, the digest container is not cleared
    expected = "13252f0a129e6c30d474dcbde5290ece336a82233ca7fd95e2c3b664fbf68384";
    input = ByteVec1D(1000, 0xff);
    blake256(input, digest);

    // Should not be equal to the first call
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(blake256, performance_call_64_bytes_input) {
    ByteVec1D input(64, 0xaa);
    ByteVec1D digest;
    PERFORMANCE_TEST(ONE_MILLION, {
        blake256(input, digest);
    })
}

} // namespace iden3math::hash
