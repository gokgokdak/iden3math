import unittest
from iden3math import Fp1
from iden3math import prime


class TestFp1(unittest.TestCase):

    def setUp(self):
        self.primes: list[int] = [
            17,
            2 ** 31 - 1, # Mersenne
            prime.bn254(), # bn254
        ]

    def test_mod_reduce(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertEqual(f.mod_reduce(0), 0)
            self.assertEqual(f.mod_reduce(1), 1)
            self.assertEqual(f.mod_reduce(-1), p - 1)
            self.assertEqual(f.mod_reduce(p - 1), p - 1)
            self.assertEqual(f.mod_reduce(p), 0)
            self.assertEqual(f.mod_reduce(p + 1), 1)
            self.assertEqual(f.mod_reduce(-p), 0)
            self.assertEqual(f.mod_reduce(-p - 1), p - 1)
            self.assertEqual(f.mod_reduce(-p + 1), 1)

    def test_add(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertEqual(f.add(0, 0), 0)
            self.assertEqual(f.add(2, 3), 5)
            self.assertEqual(f.add(-2, 3), 1)
            self.assertEqual(f.add(2, -3), p - 1)
            self.assertEqual(f.add(-2, -3), p - 5)
            self.assertEqual(f.add(p, 0), 0)
            self.assertEqual(f.add(p, 1), 1)
            self.assertEqual(f.add(p - 1, 1), 0)
            self.assertEqual(f.add(p - 2, 5), 3)
            self.assertEqual(f.add(p - 2, -5), p - 7)

    def test_sub(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertEqual(f.sub(0, 0), 0)
            self.assertEqual(f.sub(5, 3), 2)
            self.assertEqual(f.sub(3, 5), p - 2)
            self.assertEqual(f.sub(-5, 3), p - 8)
            self.assertEqual(f.sub(-3, 5), p - 8)
            self.assertEqual(f.sub(p, 0), 0)
            self.assertEqual(f.sub(p, 1), p - 1)
            self.assertEqual(f.sub(p + 1, 1), 0)
            self.assertEqual(f.sub(p + 2, 3), p - 1)
            self.assertEqual(f.sub(p - 1, 1), p - 2)
            self.assertEqual(f.sub(p - 2, 5), p - 7)
            self.assertEqual(f.sub(p - 2, -5), 3)

    def test_mul(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertEqual(f.mul(0, 0), 0)
            self.assertEqual(f.mul(2, 3), 6)
            self.assertEqual(f.mul(-2, 3), p - 6)
            self.assertEqual(f.mul(2, -3), p - 6)
            self.assertEqual(f.mul(-2, -3), 6)
            self.assertEqual(f.mul(p, 0), 0)
            self.assertEqual(f.mul(p, 1), 0)
            self.assertEqual(f.mul(p + 1, 2), 2)
            self.assertEqual(f.mul(p + 2, 3), 6)
            self.assertEqual(f.mul(p - 1, 2), p - 2)
            self.assertEqual(f.mul(p - 2, 3), p - 6)
            self.assertEqual(f.mul(0, 5), 0)
            self.assertEqual(f.mul(-1, 5), p - 5)

    def test_div(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertIsNone(f.div(1, 0))
            self.assertEqual(f.div(1, p - 1), p - 1)
            self.assertEqual(f.div(1, 1), 1)
            self.assertEqual(f.div(4, 2), 2)
            self.assertEqual(f.div(6, 2), 3)
            self.assertEqual(f.div(6, 3), 2)
            self.assertEqual(f.div(8, 4), 2)
            self.assertEqual(f.div(8, -4), p - 2)
            self.assertEqual(f.div(-8, 4), p - 2)
            self.assertEqual(f.div(-8, -4), 2)
            # 5 / 3
            x = f.div(5, 3)
            self.assertIsNotNone(x)
            self.assertEqual(f.mul(x, 3), 5)
            # 2 / 4
            x = f.div(2, 4)
            self.assertIsNotNone(x)
            self.assertEqual(2, f.mul(x, 4))
            # (p - 1) / (p - 2)
            x = f.div(p - 1, p - 2)
            self.assertIsNotNone(x)
            self.assertEqual(p - 1, f.mul(x, p - 2))
            # (p - 2) / (p - 1)
            x = f.div(p - 2, p - 1)
            self.assertIsNotNone(x)
            self.assertEqual(p - 2, f.mul(x, p - 1))
            # div by zero
            x = f.div(1, 0)
            self.assertIsNone(x)

    def test_pow(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertEqual(f.pow(2, 3), 8)
            self.assertEqual(f.pow(2, 0), 1)
            self.assertEqual(f.pow(0, 5), 0)
            self.assertEqual(f.pow(5, 1), 5)
            # 2 ^ (-3)
            x = f.pow(2, -3)
            self.assertIsNotNone(x)
            self.assertEqual(f.mod_reduce(x * f.pow(2, 3)), 1)
            # (-2) ^ 3
            x = f.pow(-2, 3)
            self.assertIsNotNone(x)
            self.assertEqual(x, f.pow(p - 2, 3)) # (-2) ^ 3 ≡ (p - 2) ^ 3
            # (-2) ^ (-3)
            x = f.pow(-2, -3)
            self.assertIsNotNone(x)
            self.assertEqual(f.mod_reduce(x * f.pow(p - 2, 3)), 1)
            # 0 ^ (-1)
            x = f.pow(0, -1)
            self.assertIsNone(x)
            # 1 ^ 0
            x = f.pow(1, 0)
            self.assertEqual(x, 1)
            # 1 ^ (-1)
            x = f.pow(1, -1)
            self.assertEqual(x, 1)

    def test_square(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertEqual(f.square(0), 0)
            self.assertEqual(f.square(1), 1)
            self.assertEqual(f.square(2), 4)
            self.assertEqual(f.square(3), 9)
            self.assertEqual(f.square(4), 16)
            self.assertEqual(f.square(-4), 16)
            self.assertEqual(f.square(p), 0)
            self.assertEqual(f.square(p - 1), 1)
            self.assertEqual(f.square(p - 2), 4)
            self.assertEqual(f.square(p - 3), 9)
            self.assertEqual(f.square(p + 1), 1)
            self.assertEqual(f.square(p + 2), 4)
            self.assertEqual(f.square(p + 3), 9)

    def test_sqrt(self):
        for p in self.primes:
            f = Fp1(p)
            # a = 0
            x = f.sqrt(0)
            self.assertEqual(x, 0)
            # a = 0 ~ 16
            for i in range(17):
                x = f.sqrt(i)
                if None == x:
                    continue
                self.assertEqual(i, f.square(x))
            # a = p +- offset
            vec_a: list[int] = [p, p * 2, p * 2 + 1, p - 2, p - 1, p + 1, p + 2]
            vec_x: list[int] = [0,     0,         1, p - 2, p - 1,     1,     2]
            for i in range(len(vec_a)):
                x = f.sqrt(vec_a[i])
                if None != x:
                    self.assertEqual(vec_x[i], f.square(x))
        # a = -1
        f = Fp1(17)
        x = f.sqrt(-1)
        self.assertIsNotNone(x)
        self.assertEqual(16, f.square(x))

    def test_mod_inv(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertIsNone(f.mod_inv(0))
            self.assertIsNone(f.mod_inv(p))
            self.assertIsNone(f.mod_inv(-p))
            self.assertEqual(f.mul(f.mod_inv(1), 1), 1)
            self.assertEqual(f.mul(f.mod_inv(-1), -1), 1)
            self.assertEqual(f.mul(f.mod_inv(-2), -2), 1)
            self.assertEqual(f.mul(f.mod_inv(2), 2), 1)
            self.assertEqual(f.mul(f.mod_inv(p - 1), p - 1), 1)
            self.assertEqual(f.mul(f.mod_inv(p + 1), p + 1), 1)

    def test_neg(self):
        for p in self.primes:
            f = Fp1(p)
            self.assertEqual(f.neg(0), 0)
            self.assertEqual(f.neg(1), p - 1)
            self.assertEqual(f.neg(2), p - 2)
            self.assertEqual(f.neg(-2), 2)
            self.assertEqual(f.neg(p), 0)
            self.assertEqual(f.neg(p - 1), 1)
            self.assertEqual(f.neg(p - 2), 2)
            self.assertEqual(f.neg(p + 1), p - 1)
            self.assertEqual(f.neg(p + 2), p - 2)

    def test_has_sqrt(self):
        f = Fp1(17)
        with_sqrt: list[int] = [1, 2, 4, 8, 9, 13, 15, 16]
        no_sqrt: list[int] = [3, 5, 6, 7, 10, 11, 12, 14]
        for a in with_sqrt:
            self.assertTrue(f.has_sqrt(a), f"Should have sqrt: {a}")
        for a in no_sqrt:
            self.assertFalse(f.has_sqrt(a), f"Should not have sqrt: {a}")
