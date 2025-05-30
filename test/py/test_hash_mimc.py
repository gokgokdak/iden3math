import unittest
import threading
from iden3math import Endian
from iden3math import hash
from iden3math import prime


class TestHashMiMCSponge(unittest.TestCase):

    def test_preimage_count_2_digest_count_3(self):
        expected = [
            "2bcea035a1251603f1ceaf73cd4ae89427c47075bb8e3a944039ff1e3d6d2a6f",
            "2f7d340a3c24b8ef9899ab5f019b85b87354c7f6c965a19ca090321f7e5425e9",
            "0cf71423c39e70b9858eaa8e1dc3ac40a09c3927dc31d12014af16066f2bdcb6",
        ]
        preimages = [
            bytearray([0x01]),
            bytearray([0x02]),
        ]
        key = bytearray()
        digests = hash.mimc_sponge(preimages, len(expected), key)
        self.assertEqual(len(expected), len(digests))
        for i in range(len(expected)):
            self.assertEqual(expected[i], digests[i].hex())

    def test_preimage_count_2_digest_count_3_key_val_0x09(self):
        expected = [
            "1a18b5636f54e0268de295548e1ddc4ce94f930ccc7ca67422d21cfb43f70a77",
            "10c7e728e6219df6431c1e55c788e777d339815553bd8c2560a628288229286f",
            "1208620a5c351a5a88dfdb6d7386d1069081d21f4cccce630edb0cb7196009c1",
        ]
        preimages = [
            bytearray([0x01]),
            bytearray([0x02]),
        ]
        key = bytearray([0x09])
        digests = hash.mimc_sponge(preimages, len(expected), key)
        self.assertEqual(len(expected), len(digests))
        for i in range(len(expected)):
            self.assertEqual(expected[i], digests[i].hex())

    def test_preimage_and_key_prime_minus_1(self):
        expected = [
            "12af600bcba99cb94f3720cdbf071d86b090bc3571bc9a91d58fc7213e444741",
            "25e469d12f79c571a2ba5c3aac42b32c1b83ff49afab33298abc2184148cac0e",
            "01dab6a98cca214537bf2ca607f3475ce2a5a64b9540dec4b14279b706060cdf",
        ]
        prime_m1: str = hex(prime.bn254() - 1).lstrip('0x')
        prime_m1 = '0' if len(prime_m1) % 2 != 0 else '' + prime_m1
        preimages = [
            bytearray.fromhex(prime_m1),
            bytearray.fromhex(prime_m1),
        ]
        key = bytearray.fromhex(prime_m1)
        digests = hash.mimc_sponge(preimages, len(expected), key)
        self.assertEqual(len(expected), len(digests))
        for i in range(len(expected)):
            self.assertEqual(expected[i], digests[i].hex())

    def test_preimage_prime_and_prime_plus_1_key_prime_plus_2(self):
        expected = [
            "068fac99307e94389058d13efa9b1ff21aaa66fc6bf2fed373f5f1e32bffe2e5",
            "18aeef5dc576db758b5d2acac9a75ddcf0671862f6709f7f0eab822d02031166",
            "151770cf9b1209c4a3fe5fd5ef8f81dd4ab555787d7e5ae7c7e7d976a15763a3",
        ]
        prime_p1: str = hex(prime.bn254() + 1).lstrip('0x')
        prime_p1 = '0' if len(prime_p1) % 2 != 0 else '' + prime_p1
        prime_p2: str = hex(prime.bn254() + 2).lstrip('0x')
        prime_p2 = '0' if len(prime_p2) % 2 != 0 else '' + prime_p2
        preimages = [
            bytearray.fromhex(hex(prime.bn254()).lstrip('0x')),
            bytearray.fromhex(prime_p1),
        ]
        key = bytearray.fromhex(prime_p2)
        digests = hash.mimc_sponge(preimages, len(expected), key)
        self.assertEqual(len(expected), len(digests))
        for i in range(len(expected)):
            self.assertEqual(expected[i], digests[i].hex())

    def test_preimage_and_key_prime_in_count_eq_out_count(self):
        expected_0 = "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581"
        expected_219 = "0b437ab068775e930f36c52caea8491e1aa1fc5260ce51c03aa261d4d51ff81c"
        prime_bn254: str = hex(prime.bn254()).lstrip('0x')
        prime_bn254 = '0' if len(prime_bn254) % 2 != 0 else '' + prime_bn254
        preimages = [
            bytearray.fromhex(prime_bn254),
            bytearray.fromhex(prime_bn254),
        ]
        key = bytearray.fromhex(prime_bn254)
        digests = hash.mimc_sponge(preimages, 220, key)
        self.assertEqual(220, len(digests))
        self.assertEqual(expected_0, digests[0].hex())
        self.assertEqual(expected_219, digests[219].hex())

    def multi_thread_job0(self, loop_count: int):
        expected = [
            "2bcea035a1251603f1ceaf73cd4ae89427c47075bb8e3a944039ff1e3d6d2a6f",
        ]
        preimages = [
            bytearray([0x01]),
            bytearray([0x02]),
        ]
        key = bytearray()
        digests = []
        for _ in range(loop_count):
            digests = hash.mimc_sponge(preimages, len(expected), key)
        self.assertEqual(len(expected), len(digests))
        for i in range(len(expected)):
            self.assertEqual(expected[i], digests[i].hex())

    def multi_thread_job1(self, loop_count: int):
        expected_0 = "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581"
        prime_bn254: str = hex(prime.bn254()).lstrip('0x')
        prime_bn254 = '0' if len(prime_bn254) % 2 != 0 else '' + prime_bn254
        preimages = [
            bytearray.fromhex(prime_bn254),
            bytearray.fromhex(prime_bn254),
        ]
        key = bytearray.fromhex(prime_bn254)
        digests = []
        for _ in range(loop_count):
            digests = hash.mimc_sponge(preimages, 1, key)
        self.assertEqual(1, len(digests))
        self.assertEqual(expected_0, digests[0].hex())

    def test_multi_thread_call(self):
        loop_count = 10000
        # Create threads for the two jobs
        worker0 = threading.Thread(target=self.multi_thread_job0, args=(loop_count,))
        worker1 = threading.Thread(target=self.multi_thread_job1, args=(loop_count,))
        # Start the threads
        worker0.start()
        worker1.start()
        # Wait for both threads to finish
        worker0.join()
        worker1.join()

    def test_preimage_val_0xff_2x32_bytes(self):
        expected = [
            "19f45c7a923c585cbc59505939367acd7960685fc90d661752b1fccd32ae6fd0",
            "0fa61f33087dfcc04b2d920826e744a85f42dee1f9e019d961cc2bf5342e9b0c",
            "17bfe4bdadbe239c72b96e97fd37f122169137fb1591242060ecac771f10a679",
        ]
        preimages = [
            bytearray.fromhex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
            bytearray.fromhex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
        ]
        key = bytearray.fromhex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff")
        digests = hash.mimc_sponge(preimages, len(expected), key)
        self.assertEqual(len(expected), len(digests))
        for i in range(len(expected)):
            self.assertEqual(expected[i], digests[i].hex())

    def test_digest_31_pad_to_32_bytes_under_big_little_endian(self):
        # Check big endian digest padding
        expected = [
            "04030c4eec2927d034bb5a92e413b7fa6f67d9a05d542f96983a860d57cb085d",
            "00b38029c8451ff2f4a9c5298aed364d2a978bdb26c7ddd3573b739e9ce68000",  # Big endian, pad 0x00 before 0xb380...8000
            "28b6ea95e68dd403f66be53f8af9a4a7185bcbf0675f077ff132056214a5cc8e",
        ]
        preimages = [
            bytearray.fromhex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            bytearray.fromhex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
        ]
        key = bytearray.fromhex("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
        digests = hash.mimc_sponge(preimages, len(expected), key, Endian.BE, Endian.BE, Endian.BE)
        self.assertEqual(len(expected), len(digests))
        for i in range(len(expected)):
            self.assertEqual(expected[i], digests[i].hex())

        # Check little endian digest padding
        expected = [
            bytearray.fromhex("04030c4eec2927d034bb5a92e413b7fa6f67d9a05d542f96983a860d57cb085d"),
            bytearray.fromhex("00b38029c8451ff2f4a9c5298aed364d2a978bdb26c7ddd3573b739e9ce68000"),
            bytearray.fromhex("28b6ea95e68dd403f66be53f8af9a4a7185bcbf0675f077ff132056214a5cc8e"),
        ]
        for i in range(len(expected)):
            expected[i].reverse()
        for i in range(len(expected)):
            expected[i] = expected[i].hex()
        digests = hash.mimc_sponge(preimages, len(expected), key, Endian.BE, Endian.BE, Endian.LE)
        self.assertEqual(len(expected), len(digests))
        for i in range(len(expected)):
            self.assertEqual(expected[i], digests[i].hex())

    def test_digest_not_cleared_and_call_hash_func_again(self):
        # First call, leave a digest in the container
        expected = [
            "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581",
        ]
        prime_bn254: str = hex(prime.bn254()).lstrip('0x')
        prime_bn254 = '0' if len(prime_bn254) % 2 != 0 else '' + prime_bn254
        preimages = [
            bytearray.fromhex(prime_bn254),
            bytearray.fromhex(prime_bn254),
        ]
        key = bytearray()
        digests = hash.mimc_sponge(preimages, 1, key)
        self.assertEqual(1, len(digests))
        self.assertEqual(expected[0], digests[0].hex())

        # Second call, the digest container is not cleared
        expected = [
            "1a18b5636f54e0268de295548e1ddc4ce94f930ccc7ca67422d21cfb43f70a77",
        ]
        preimages = [
            bytearray([0x01]),
            bytearray([0x02]),
        ]
        key = bytearray([0x09])
        digests = hash.mimc_sponge(preimages, len(expected), key)
        self.assertEqual(len(expected), len(digests))
        self.assertEqual(1, len(digests))

        # Should not be equal to the first call
        self.assertEqual(expected[0], digests[0].hex())
