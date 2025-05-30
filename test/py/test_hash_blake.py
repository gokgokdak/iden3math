import unittest
from iden3math import hash


class TestHashBlake256(unittest.TestCase):

    def test_empty_input(self):
        input = b''
        digest = hash.blake256(input)
        self.assertTrue(len(digest) == 0)
        input = ''
        digest = hash.blake256(input)
        self.assertTrue(len(digest) == 0)

    def test_single_byte_0x00(self):
        expected = "0ce8d4ef4dd7cd8d62dfded9d4edb0a774ae6a41929a74da23109e8f11139c87"
        input = bytes([0x00])
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_single_byte_0xaa(self):
        expected = "2057dc48805e7e0a1140f2a80e72d58dd9607dfeffcb0159b98658fc15322c1e"
        input = bytes([0xaa])
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_single_byte_0xff(self):
        expected = "63bf0367c7edf0a5285db26daaeb90629434ce05e7f385de1db83ea3c70bc4cf"
        input = bytes([0xff])
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_size_minus_one_val_0x00(self):  # 55 bytes (block size - 1 for padding byte)
        expected = "dc980544f4181cc43505318e317cdfd4334dab81ae035a28818308867ce23060"
        input = bytes([0x00] * 55)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_size_minus_one_val_0xaa(self):
        expected = "2cd6a7d2bc557e771cc7b482f987cb18ffd5d2d19cf444ff03f1fcc736da44a4"
        input = bytes([0xaa] * 55)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_size_minus_one_val_0xff(self):
        expected = "d806c129c0a95654d746419667a9f0878da9cc5d55d77e3e22df7c1b12176010"
        input = bytes([0xff] * 55)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_one_full_val_0x00(self):  # 64 bytes
        expected = "6d994042954f8dc5633626cd50b2bc66d733a313d67fd9702c5a8149a8028c98"
        input = bytes([0x00] * 64)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_one_full_val_0xaa(self):
        expected = "00bb5fa2cd91c47bb8bfaf8f8489a2603d7b7d7291894d476d4858961436ae9a"
        input = bytes([0xaa] * 64)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_one_full_val_0xff(self):
        expected = "80a0ace8b131870da8de11bca85a811f44ece342c57cb8cd5567d2a33685b5be"
        input = bytes([0xff] * 64)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_one_full_and_36_bytes_val_0x00(self):  # 100 bytes (1.5625 blocks)
        expected = "db10fa7d8a13c4bb74729474485366132da7e221ec651f57a3b7fc3258af9696"
        input = bytes([0x00] * 100)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_one_full_and_36_bytes_val_0xaa(self):
        expected = "cf274ed07d6e716f979a238151bb9d6c6a67376e5b736046880a30819ee3d3fe"
        input = bytes([0xaa] * 100)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_one_full_and_36_bytes_val_0xff(self):
        expected = "c9dda01aa7cd6e0c5401ac0d3c4a30abab4c229d7039714be52db6257088c7ae"
        input = bytes([0xff] * 100)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_two_full_val_0x00(self):  # 128 bytes
        expected = "4c8ed99ae2cfdd5bdaba9f19848fcd98b4c60e122096a47ea565c410a1d567ce"
        input = bytes([0x00] * 128)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_two_full_val_0xaa(self):
        expected = "5320a6e09347384b05bbe61b5d43bb2055584a56c1f753a9ac93cecff0a271c8"
        input = bytes([0xaa] * 128)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_block_two_full_val_0xff(self):
        expected = "9f0edf629f1045114aedcbe094093e4f4e7a4b24ca9794219a35499a4969f1d9"
        input = bytes([0xff] * 128)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_long_message_1000_bytes_val_0x00(self):  # 1000 bytes to test multiple block processing and length overflow
        expected = "e63f5343fc28b480dd8e9586f5cc9b11827e9b317d7a4086326e671b68a5efee"
        input = bytes([0x00] * 1000)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_long_message_1000_bytes_val_0xaa(self):
        expected = "a821641b802939d182455e571294313674e36877ab362c589728ae182458c513"
        input = bytes([0xaa] * 1000)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_long_message_1000_bytes_val_0xff(self):
        expected = "13252f0a129e6c30d474dcbde5290ece336a82233ca7fd95e2c3b664fbf68384"
        input = bytes([0xff] * 1000)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

    def test_text_input(self):
        input0 = bytes([0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39])
        input1 = "0123456789"
        digest0 = hash.blake256(input0)
        digest1 = hash.blake256(input1.encode('utf-8'))
        self.assertEqual(digest0.hex(), digest1.hex())

    def test_digest_not_cleared_and_call_hash_func_again(self):
        # First call, leave a digest in the container
        expected = "a821641b802939d182455e571294313674e36877ab362c589728ae182458c513"
        input = bytes([0xaa] * 1000)
        digest = hash.blake256(input)
        self.assertEqual(expected, digest.hex())

        # Second call, the digest container is not cleared
        expected = "13252f0a129e6c30d474dcbde5290ece336a82233ca7fd95e2c3b664fbf68384"
        input = bytes([0xff] * 1000)
        digest = hash.blake256(input)

        # Should not be equal to the first call
        self.assertEqual(expected, digest.hex())

    def test_performance_call_64_bytes_input(self):
        input = bytes([0xaa] * 64)
        digest = hash.blake256(input)
        # This is a performance test, so we won't assert anything here.
        # The test will pass if it runs without errors.
        self.assertIsNotNone(digest)
