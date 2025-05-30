#include <iden3math/hash/keccak.h>
#include <iden3math/serialize.h>
#include <gtest/gtest.h>
#include "../helper.h"

namespace iden3math::hash {

TEST(keccak256, text_0) {
    auto preimage = "keccak256";
    auto expected = "b7845733ba102a68c6eb21c3cd2feafafd1130de581d7e73be60b76d775b6704";

    ByteVec1D digest;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, text_1) {
    auto preimage = "Transfer(address,address,uint256)";
    auto expected = "ddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef";

    ByteVec1D digest;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, digest_not_cleared_and_call_hash_func_again) {
    auto preimage = "keccak256";
    auto expected = "b7845733ba102a68c6eb21c3cd2feafafd1130de581d7e73be60b76d775b6704";
    ByteVec1D digest;
    std::string digest_str;

    keccak256(preimage, digest);
    digest_str = serialize::bytes_to_hexstr(digest);
    EXPECT_EQ(expected, digest_str);

    preimage = "Transfer(address,address,uint256)";
    expected = "ddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef";

    keccak256(preimage, digest);
    digest_str = serialize::bytes_to_hexstr(digest);
    EXPECT_EQ(expected, digest_str);
}

TEST(keccak256, same_container_for_input_and_output) {
    auto seed = "keccak256";
    auto expected = "c0168e0d8493e7a9939bce2051cd56fa67ad757c9af47ad0232d4a39ae760dd8";
    ByteVec1D digest;

    keccak256(seed, digest);
    keccak256(digest, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_1_bytes_pad_to_32_bytes) {
    ByteVec1D digest;
    std::string error;

    // 0x00000000000000000000000000000000000000000000000000000000000000ff
    ByteVec1D preimage = {0xff};
    serialize::pad(preimage, 0x00, 31, true);
    keccak256(preimage, digest);
    EXPECT_EQ("e08ec2af2cfc251225e1968fd6ca21e4044f129bffa95bac3503be8bdb30a367", serialize::bytes_to_hexstr(digest));

    // 0xff00000000000000000000000000000000000000000000000000000000000000
    preimage = {0xff};
    serialize::pad(preimage, 0x00, 31, false);
    keccak256(preimage, digest);
    EXPECT_EQ("a282094698c0c30b930221fa2e1311ac7871cd86e61417eb71ac92e3d58ba4da", serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, number_0x00) {
    auto preimage = BigInt("00", 16);
    auto expected = "bc36789e7a1e281436464229828f817d6612f7b477d66591ff96a9e064bcc98a";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_0_bytes) {
    ByteVec1D preimage = {};
    auto expected = "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470";

    ByteVec1D digest;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_1_bytes) {
    auto preimage = BigInt("01", 16);
    auto expected = "5fe7f977e71dba2ea1a68e21057beebb9be2ac30c6410aa38d4f3fbe41dcffd2";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_2_bytes) {
    auto preimage = BigInt("0123", 16);
    auto expected = "667d3611273365cfb6e64399d5af0bf332ec3e5d6986f76bc7d10839b680eb58";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_3_bytes) {
    auto preimage = BigInt("012345", 16);
    auto expected = "aa4ba4b304228a9d05087e147c9e86d84c708bbbe62bb35b28dab74492f6c726";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_4_bytes) {
    auto preimage = BigInt("01234567", 16);
    auto expected = "652d86dcb7eecf8be8bf4f7fba8cdc4f9b3dddaeebbd5afdf530371de63c0a99";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_5_bytes) {
    auto preimage = BigInt("0123456789", 16);
    auto expected = "79fad56e6cf52d0c8c2c033d568fc36856ba2b556774960968d79274b0e6b944";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_6_bytes) {
    auto preimage = BigInt("0123456789ab", 16);
    auto expected = "e2e02bc63edb7760389aee0d233f7d8d75237e03b81d6197f2ef1269ae547576";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_7_bytes) {
    auto preimage = BigInt("0123456789abcd", 16);
    auto expected = "d3c552a3b8b0452e4991ff4cd90c1aa1cdaa059826772e20ae4ced5ff5247010";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_8_bytes) {
    auto preimage = BigInt("0123456789abcdef", 16);
    auto expected = "0c3d72390ac0ce0233c551a3c5278f8625ba996f5985dc8d612a9fc55f1de15a";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_9_bytes) {
    auto preimage = BigInt("0123456789abcdef01", 16);
    auto expected = "6e2d7e7a5b156cee60602ef6fde9a003b3410633c9e3fcc65611a89afdc2e7e0";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_32_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 16);
    auto expected = "f5e0be5ec5f0820f83decedf083468519a4e3c1c7568ab0a5f4ed56a2d5037a4";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_9_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcd", 16);
    auto expected = "e6ceb7d956ae7ea58ee995db9daed5a801adb6580b6b8f8d137cec1d81d7f012";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_8_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 16);
    auto expected = "2acdfc5b5522eb5fddb177479fa66c965d44470e83c55928b5332966acef9bde";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_7_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01", 16);
    auto expected = "05dbf308c902e6cde7effc6fa52940b1bc60754500830d59f1c5b998bc3c2d96";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_6_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123", 16);
    auto expected = "749d0dae56d7d4fdb0578780510c7347c2a4c04acd40d7e320d99dc792f162a5";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_5_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef012345", 16);
    auto expected = "b3072dc3f354380c15083d809387292cbf750810e038fc3065b37d6a4df14612";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_4_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567", 16);
    auto expected = "984165fb33e1f839a80dcea669789604563a4fea225e7442af7c6701e191a22b";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_3_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789", 16);
    auto expected = "14d637d537f2d071574f792efc60cf0f4e7dc40cdb6af6b721371e703b5e7049";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_2_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab", 16);
    auto expected = "96dae491dced22bc4c9871ff144fc2c37c446797754a5b9e645559d65474812c";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_minus_1_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcd", 16);
    auto expected = "3e4916729e2522af4937548f5848a5b49067eec910a0a6a890b0c71dde08854e";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 16);
    auto expected = "3f7424fa94a2f8c5a733b86dac312d85685f9af3dea919694cc6a8abfc075460";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_1_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01", 16);
    auto expected = "bd039942db13a90eb8d5ee6e98c2af187e77bc1884d622e3d47fb573e80cefc2";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_2_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123", 16);
    auto expected = "4292ca8cca198d9e7c7b9faaeda70025d1cd422ac8fae5f52ec03a21b6199e6d";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_3_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef012345", 16);
    auto expected = "a8073f8bc83c1fc173ae747a1b217a894203678ee22f9e3b13199eb5df3b76fc";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_4_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567", 16);
    auto expected = "08f8d877b7332fc4e87c0551356501d7c7d005ff1d13790a7101f42ffc6a6025";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_5_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789", 16);
    auto expected = "f504209429ed038f73e928b93e60e1653823b2a4c08b486b8e37628efdcee71e";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_6_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab", 16);
    auto expected = "b1d3bdc6f4919c636c12da601a2f461e600dc64b5d11b16c8e006edda24f69c2";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_7_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcd", 16);
    auto expected = "f96d97e3f7e634c63bba61fbebb433060f97c4cf47583313fbb54731a4a16429";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_8_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 16);
    auto expected = "bf752058f36f3f2165a8942fb07484fd94509eab97e60f3cb1824e425c5a8caf";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136_plus_9_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01", 16);
    auto expected = "38d0de9a467365424c9f11326d690bbc024283859772af2a3bddcc747c89d4f0";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_9_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcd", 16);
    auto expected = "c9d0c4f5ae0f22eb76e8f5fa60d92f2c1c087ab7eb5f81489f33025387505fe2";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_8_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 16);
    auto expected = "5bbe6b4e86fdd5efec5b11e9eafb228b5b94f091790d2534ec565a8c54f00dd3";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_7_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01", 16);
    auto expected = "11641540ae8c9221c7ccbb7adad2e3adb4936ae7ccd18f05f784030dba8d0cad";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_6_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123", 16);
    auto expected = "7657e6402fd451bac4b96f89fee0c7cc169a6751486c1c4015435a0d3898703f";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_5_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef012345", 16);
    auto expected = "229d1feae90dc0c71a51940965c73c8ce3e5b5055f899e03dc8607f9fae7cdaf";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_4_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567", 16);
    auto expected = "b82ba8d9064ab4030dee1a5a907c5c109034c671425d8a7a778602454885efba";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_3_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789", 16);
    auto expected = "cbffcd0eed2b92b8df590fd545b374c2ff73b61d1e125dc669499d3488f35489";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_2_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab", 16);
    auto expected = "62e6bd6a4bfa901e8cc4a4bcce78acfaef4977362f4b1a3f965a3d6e3921d840";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_minus_1_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcd", 16);
    auto expected = "d1ee150ef029d86324d5f96055eb6732f54275c05665d0e73830aaa67688add1";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 16);
    auto expected = "e8bfa50dd3d49e950a93c0369b6045f1ae418c1e728582974d295fd8b57945f6";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_1_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01", 16);
    auto expected = "6375a95f36969022961a34ef2dd5bc9bd1b91a630c11985b179095480affd4b8";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_2_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123", 16);
    auto expected = "5fd95ea2bf266e363b61a8452f6b963ffe246d04196b9861c4b5a4f668c1ca83";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_3_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef012345", 16);
    auto expected = "774b9925a7558f5c39f84f7aa74d0e8252baf8fb5a5c9b3d46513367483b7887";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_4_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01234567", 16);
    auto expected = "22a4f90af5fd121969752b19beb628d70991f1b833157859f32a4060b561130b";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_5_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789", 16);
    auto expected = "319e73fbbb347fc3dae38bdf7244874e4545a35389f8a985af6ed1e085c40290";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_6_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789ab", 16);
    auto expected = "b2616d4a3fca77610896d1395454f69abee05b49cb324aa863459294f9bbeb8c";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_7_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcd", 16);
    auto expected = "cd67501890a6abab56d3974cffab263d31df302ec225205d6459907b13035ef7";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_8_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 16);
    auto expected = "8e9b8db6cf9accee40ce994742abc3fb729682af1c4c1dcec10d24756786dce3";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_136x2_plus_9_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef01", 16);
    auto expected = "0e9ad5950b77c84da66a8b7e91447a757fad7770522eacdbf116e23da7450e22";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, preimage_len_5440_bytes) {
    auto preimage = BigInt("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 16);
    auto expected = "1a66fd6d2d37cb05ba1d68d3c1a7bf93e0430e907495b4351f4eec25f882b751";

    ByteVec1D digest;
    std::string error;
    keccak256(preimage, digest);
    EXPECT_EQ(expected, serialize::bytes_to_hexstr(digest));
}

TEST(keccak256, peformance_call_64_bytes_input) {
    ByteVec1D input(64, 0xaa);
    ByteVec1D digest;
    PERFORMANCE_TEST(ONE_MILLION, {
        keccak256(input, digest);
    })
}

} // namespace iden3math::hash
