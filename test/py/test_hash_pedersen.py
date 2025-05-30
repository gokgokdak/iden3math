import unittest
from iden3math import hash


class TestHashPedersen(unittest.TestCase):

    def test_empty_input(self):
        preimage = bytearray()
        digest = bytes([0x01])
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_single_byte_input(self):
        preimage = bytearray([0x01])
        digest = bytes.fromhex("75c28cc0b8c45fa951bd48ffeb096e3373dac173ee78fc7b58e9ce8dc193b01d")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_two_byte_input(self):
        preimage = bytearray([0x01, 0x01])
        digest = bytes.fromhex("70db4185b286d4f8cbc8b7bc8b72e312c3b8eb508d3d61e4cf5e8d59d5a81ba6")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_exactly_one_segment(self):
        # One segment = WINDOW_SIZE * WINDOWS_PER_SEG = 4 * 50 = 200 bits = 25 bytes
        preimage = bytearray([0xaa] * 25)
        digest = bytes.fromhex("52d57e2313839d26950b857c0ee3e9b938cfd928a066ff30c301bf77cc705c06")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_one_segment_plus_one_bit(self):
        # 201 bits = 26 bytes (last byte only uses 1 bit)
        preimage = bytearray([0xaa] * 25) + bytearray([0x01])
        digest = bytes.fromhex("b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_common_32_bytes_input(self):
        preimage = bytearray.fromhex("b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3")
        digest = bytes.fromhex("5e749cf88b25a21a292a3116c8e068af236ac370c51fe8ab96291c68392392a2")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_common_32_bytes_input_0x00_prefix_suffix(self):
        preimage = bytearray.fromhex("00ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f7632880800")
        digest = bytes.fromhex("f3d0d29c8ca8cb52845f2b02e2dd61b494dd40ba19cfde518d8b1161a6260c29")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_common_64_bytes_input(self):
        preimage = bytearray.fromhex("b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3")
        digest = bytes.fromhex("de1a2202abf4c240d871b13f547011c7fca48e258d8c0ae82745e5ceb9e1b522")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_common_64_bytes_input_0x00_prefix_suffix(self):
        preimage = bytearray.fromhex("00ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f7632880800")
        digest = bytes.fromhex("b634ccf13fdba7b9c2666167869686910e6035f37bb31ba37e7df94e20f8801a")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_exactly_two_segments(self):
        # Two segments = 400 bits = 50 bytes
        preimage = bytearray([0xaa] * 50)
        digest = bytes.fromhex("09552440bd1c1bab3003021b564fddcab835ed6248f157d81c3df7581452531c")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_exactly_two_segments_plus_one_bit(self):
        # Two segments = 400 bits = 50 bytes
        preimage = bytearray([0xaa] * 50) + bytearray([0x01])
        digest = bytes.fromhex("cbcc6877d2bc7dab56621533f4ce2a5b91be9b842b2ae561fa7546251cfe4e2e")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_large_input_2000_bytes(self):
        preimage = bytearray([0xaa] * 1000)
        seg = bytearray([0xcd] * 1000)
        preimage.extend(seg)
        digest = bytes.fromhex("1bf524b6681272083e9014f73ec4093399270d1ea4aeae96c623ec7e34f2a48d")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_all_max_value_bytes(self):
        preimage = bytearray([0xff] * 100)
        digest = bytes.fromhex("47522d13124d066e751a0254ce3a439db81263c0b543376cd27d0bb37e658a0d")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

    def test_digest_not_cleared_and_call_hash_func_again(self):
        preimage = bytearray([0xff] * 100)
        digest = bytes.fromhex("47522d13124d066e751a0254ce3a439db81263c0b543376cd27d0bb37e658a0d")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)

        preimage = bytearray([0xaa] * 50) + bytearray([0x01])
        digest = bytes.fromhex("cbcc6877d2bc7dab56621533f4ce2a5b91be9b842b2ae561fa7546251cfe4e2e")
        digest = digest.ljust(32, b'\x00')
        output = hash.pedersen(preimage)
        self.assertEqual(digest, output)
