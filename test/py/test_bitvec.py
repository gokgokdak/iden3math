import unittest
from iden3math import BitVec1D


class TestBitVec1D(unittest.TestCase):

    def test_constructor(self):
        bv = BitVec1D()
        self.assertEqual(len(bv), 0)
        self.assertEqual(bv.size(), 0)

    def test_access_out_of_index(self):
        bv = BitVec1D()
        self.assertRaises(IndexError, lambda: bv[1])
        for i in range(8):
            bv.push(True)
        self.assertTrue(bv[7])
        self.assertRaises(IndexError, lambda: bv[8])
        bv.push(True)
        self.assertTrue(bv[8])
        self.assertRaises(IndexError, lambda: bv[9])

    def test_push_and_access(self):
        bv = BitVec1D()

        # Test pushing bits
        bv.push(True)
        self.assertTrue(bv[0])

        bv.push(False)
        self.assertFalse(bv[1])

        bv.push(True)
        self.assertTrue(bv[2])

        # Test bits across byte boundaries
        for i in range(10):
            bv.push(i % 2 == 0)

        self.assertTrue(bv[3])
        self.assertFalse(bv[4])
        self.assertTrue(bv[5])
        self.assertFalse(bv[6])
        self.assertTrue(bv[7])
        self.assertFalse(bv[8])
        self.assertTrue(bv[9])
        self.assertFalse(bv[10])
        self.assertTrue(bv[11])
        self.assertFalse(bv[12])

    def test_push_and_access_byte_boundary(self):
        bv = BitVec1D()

        # Push 8 bits to fill first byte
        for i in range(8):
            bv.push(True)

        # Verify all bits in first byte
        for i in range(8):
            self.assertTrue(bv[i])

        # Push more bits to test byte boundary crossing
        bv.push(False)
        bv.push(True)

        self.assertFalse(bv[8])
        self.assertTrue(bv[9])

    def test_alternating_pattern(self):
        bv = BitVec1D()

        # Push alternating bits
        for i in range(16):
            bv.push(i % 2 == 0)

        # Verify alternating pattern
        for i in range(16):
            self.assertEqual(bv[i], i % 2 == 0)
