#include <iden3math/hash/mimc.h>
#include <iden3math/prime.h>
#include <iden3math/serialize.h>
#include <thread>
#include <gtest/gtest.h>
#include "../helper.h"

namespace iden3math::hash {

TEST(mimc_sponge, preimage_count_2_digest_count_3) {
    const std::vector<std::string> expected = {
        "2bcea035a1251603f1ceaf73cd4ae89427c47075bb8e3a944039ff1e3d6d2a6f",
        "2f7d340a3c24b8ef9899ab5f019b85b87354c7f6c965a19ca090321f7e5425e9",
        "0cf71423c39e70b9858eaa8e1dc3ac40a09c3927dc31d12014af16066f2bdcb6",
    };
    ByteVec2D preimages = {
        {0x01},
        {0x02},
    };
    ByteVec1D key = {};
    ByteVec2D digests;
    mimc_sponge(preimages, expected.size(), key, digests);

    EXPECT_EQ(expected.size(), digests.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], serialize::bytes_to_hexstr(digests[i]));
    }
}

TEST(mimc_sponge, preimage_count_2_digest_count_3_key_val_0x09) {
    std::vector<std::string> expected = {
        "1a18b5636f54e0268de295548e1ddc4ce94f930ccc7ca67422d21cfb43f70a77",
        "10c7e728e6219df6431c1e55c788e777d339815553bd8c2560a628288229286f",
        "1208620a5c351a5a88dfdb6d7386d1069081d21f4cccce630edb0cb7196009c1",
    };
    ByteVec2D preimages = {
        {0x01},
        {0x02},
    };
    ByteVec1D key = {0x09};
    ByteVec2D digests;
    mimc_sponge(preimages, expected.size(), key, digests);

    EXPECT_EQ(expected.size(), digests.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], serialize::bytes_to_hexstr(digests[i]));
    }
}

TEST(mimc_sponge, preimage_and_key_prime_minus_1) {
    std::vector<std::string> expected = {
        "12af600bcba99cb94f3720cdbf071d86b090bc3571bc9a91d58fc7213e444741",
        "25e469d12f79c571a2ba5c3aac42b32c1b83ff49afab33298abc2184148cac0e",
        "01dab6a98cca214537bf2ca607f3475ce2a5a64b9540dec4b14279b706060cdf",
    };
    ByteVec2D preimages = {
        BigInt(prime::bn254() - 1).bytes(BE),
        BigInt(prime::bn254() - 1).bytes(BE),
    };
    ByteVec1D key = BigInt(prime::bn254() - 1).bytes(BE);
    ByteVec2D digests;
    mimc_sponge(preimages, expected.size(), key, digests);

    EXPECT_EQ(expected.size(), digests.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], serialize::bytes_to_hexstr(digests[i]));
    }
}

TEST(mimc_sponge, preimage_prime_and_prime_plus_1_key_prime_plus_2) {
    std::vector<std::string> expected = {
        "068fac99307e94389058d13efa9b1ff21aaa66fc6bf2fed373f5f1e32bffe2e5",
        "18aeef5dc576db758b5d2acac9a75ddcf0671862f6709f7f0eab822d02031166",
        "151770cf9b1209c4a3fe5fd5ef8f81dd4ab555787d7e5ae7c7e7d976a15763a3",
    };
    ByteVec2D preimages = {
        BigInt(prime::bn254() + 0).bytes(BE),
        BigInt(prime::bn254() + 1).bytes(BE),
    };
    ByteVec1D key = BigInt(prime::bn254() + 2).bytes(BE);
    ByteVec2D digests;
    mimc_sponge(preimages, expected.size(), key, digests);

    EXPECT_EQ(expected.size(), digests.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], serialize::bytes_to_hexstr(digests[i]));
    }
}

TEST(mimc_sponge, preimage_and_key_prime_in_count_eq_out_count) {
    std::string expected_0 = "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581";
    std::string expected_219 = "0b437ab068775e930f36c52caea8491e1aa1fc5260ce51c03aa261d4d51ff81c";
    ByteVec2D preimages = {
        BigInt(prime::bn254()).bytes(BE),
        BigInt(prime::bn254()).bytes(BE),
    };
    ByteVec1D key = BigInt(prime::bn254()).bytes(BE);
    ByteVec2D digests;
    mimc_sponge(preimages, 220, key, digests);

    EXPECT_EQ(220, digests.size());
    EXPECT_EQ(expected_0, serialize::bytes_to_hexstr(digests[0]));
    EXPECT_EQ(expected_219, serialize::bytes_to_hexstr(digests[219]));
}

auto multi_thread_job0 = [](size_t loop_count) {
    const std::vector<std::string> expected = {
        "2bcea035a1251603f1ceaf73cd4ae89427c47075bb8e3a944039ff1e3d6d2a6f",
    };
    ByteVec2D preimages = {
        {0x01},
        {0x02},
    };
    ByteVec1D key = {};
    ByteVec2D digests;
    for (size_t _ = 0; _ < loop_count; ++_) {
        mimc_sponge(preimages, expected.size(), key, digests);
    }
    EXPECT_EQ(expected.size(), digests.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], serialize::bytes_to_hexstr(digests[i]));
    }
};

auto multi_thread_job1 = [](size_t loop_count) {
    std::string expected_0 = "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581";
    ByteVec2D preimages = {
        BigInt(prime::bn254()).bytes(BE),
        BigInt(prime::bn254()).bytes(BE),
    };
    ByteVec1D key = BigInt(prime::bn254()).bytes(BE);
    ByteVec2D digests;
    for (size_t _ = 0; _ < loop_count; ++_) {
        mimc_sponge(preimages, 1, key, digests);
    }
    EXPECT_EQ(1, digests.size());
    EXPECT_EQ(expected_0, serialize::bytes_to_hexstr(digests[0]));
};

TEST(mimc_sponge, multi_thread_call) {
    size_t loop_count = 10000;
    auto worker0 = std::thread(multi_thread_job0, loop_count);
    auto worker1 = std::thread(multi_thread_job1, loop_count);
    worker0.join();
    worker1.join();
}

TEST(mimc_sponge, preimage_val_0xff_2x32_bytes) {
    std::vector<std::string> expected = {
        "19f45c7a923c585cbc59505939367acd7960685fc90d661752b1fccd32ae6fd0",
        "0fa61f33087dfcc04b2d920826e744a85f42dee1f9e019d961cc2bf5342e9b0c",
        "17bfe4bdadbe239c72b96e97fd37f122169137fb1591242060ecac771f10a679",
    };
    ByteVec2D preimages = {
        *serialize::hexstr_to_bytes("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
        *serialize::hexstr_to_bytes("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
    };
    ByteVec1D key;
    serialize::hexstr_to_bytes("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", key);
    ByteVec2D digests;

    mimc_sponge(preimages, expected.size(), key, digests);
    EXPECT_EQ(expected.size(), digests.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], serialize::bytes_to_hexstr(digests[i]));
    }
}

TEST(mimc_sponge, digest_31_pad_to_32_bytes_under_big_little_endian) {
    // Check big endian digest padding
    std::vector<std::string> expected = {
        "04030c4eec2927d034bb5a92e413b7fa6f67d9a05d542f96983a860d57cb085d",
        "00b38029c8451ff2f4a9c5298aed364d2a978bdb26c7ddd3573b739e9ce68000", // Big endian, pad 0x00 before 0xb380...8000
        "28b6ea95e68dd403f66be53f8af9a4a7185bcbf0675f077ff132056214a5cc8e",
    };
    ByteVec2D preimages = {
        *serialize::hexstr_to_bytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
        *serialize::hexstr_to_bytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
    };
    ByteVec1D key;
    serialize::hexstr_to_bytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", key);
    ByteVec2D digests;
    Endian preimage_endian = BE;
    Endian key_endian = BE;
    Endian digest_endian = BE;

    mimc_sponge(preimages, expected.size(), key, digests, preimage_endian, key_endian, digest_endian);
    EXPECT_EQ(expected.size(), digests.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], serialize::bytes_to_hexstr(digests[i]));
    }

    // Check little endian digest padding
    expected = {
        serialize::bytes_to_hexstr(serialize::swap_endian(*serialize::hexstr_to_bytes("04030c4eec2927d034bb5a92e413b7fa6f67d9a05d542f96983a860d57cb085d"))),
        serialize::bytes_to_hexstr(serialize::swap_endian(*serialize::hexstr_to_bytes("00b38029c8451ff2f4a9c5298aed364d2a978bdb26c7ddd3573b739e9ce68000"))),
        serialize::bytes_to_hexstr(serialize::swap_endian(*serialize::hexstr_to_bytes("28b6ea95e68dd403f66be53f8af9a4a7185bcbf0675f077ff132056214a5cc8e"))),
    };
    digest_endian = LE;
    mimc_sponge(preimages, expected.size(), key, digests, preimage_endian, key_endian, digest_endian);
    EXPECT_EQ(expected.size(), digests.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], serialize::bytes_to_hexstr(digests[i]));
    }
}

TEST(mimc_sponge, digest_not_cleared_and_call_hash_func_again) {
    // First call, leave a digest in the container
    std::vector<std::string> expected = {
        "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581",
    };
    ByteVec2D preimages = {
        BigInt(prime::bn254()).bytes(BE),
        BigInt(prime::bn254()).bytes(BE),
    };
    ByteVec1D key = {};
    ByteVec2D digests;
    mimc_sponge(preimages, 1, key, digests);
    EXPECT_EQ(1, digests.size());
    EXPECT_EQ(expected[0], serialize::bytes_to_hexstr(digests[0]));

    // Second call, the digest container is not cleared
    expected = {
        "1a18b5636f54e0268de295548e1ddc4ce94f930ccc7ca67422d21cfb43f70a77",
    };
    preimages = {
        {0x01},
        {0x02},
    };
    key = {0x09};
    mimc_sponge(preimages, expected.size(), key, digests);
    EXPECT_EQ(expected.size(), digests.size());
    EXPECT_EQ(1, digests.size());

    // Should not be equal to the first call
    EXPECT_EQ(expected[0], serialize::bytes_to_hexstr(digests[0]));
}

TEST(mimc_sponge, peformance_2x32_bytes_in_32_bytes_out) {
    ByteVec2D preimages = {
        ByteVec1D(32, 0xaa),
        ByteVec1D(32, 0xaa),
    };
    ByteVec1D key = {};
    ByteVec2D digests;
    PERFORMANCE_TEST(ONE_THOUSAND, {
        mimc_sponge(preimages, 1, key, digests);
    })
}

} // namespace iden3math::hash
