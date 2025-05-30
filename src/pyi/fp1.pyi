"""
This module provides type annotations for the Fp1 class.
"""

class Fp1:
    """
    A class representing a prime field with a given prime number.
    """

    def __init__(self, prime: int) -> None:
        """
        Initialize the prime field with the given prime number.

        :param prime: The prime number for the field.
        """
        ...

    def mod_reduce(self, a: int) -> int:
        """
        Reduce the given integer modulo the field's prime.

        :param a: The integer to reduce.
        :return: The reduced integer.
        """
        ...

    def add(self, a: int, b: int) -> int:
        """
        Add two integers in the field.

        :param a: The first integer.
        :param b: The second integer.
        :return: The sum of the integers.
        """
        ...

    def sub(self, a: int, b: int) -> int:
        """
        Subtract two integers in the field.

        :param a: The first integer.
        :param b: The second integer.
        :return: The difference of the integers.
        """
        ...

    def mul(self, a: int, b: int) -> int:
        """
        Multiply two integers in the field.

        :param a: The first integer.
        :param b: The second integer.
        :return: The product of the integers.
        """
        ...

    def div(self, a: int, b: int) -> int:
        """
        Divide two integers in the field.

        :param a: The numerator integer.
        :param b: The denominator integer.
        :return: The quotient of the integers. Returns None if division by zero or 'b' does not have a modular inverse.
        """
        ...

    def pow(self, a: int, b: int) -> int:
        """
        Raise an integer to a power in the field.

        :param a: The base integer.
        :param b: The exponent integer.
        :return: The result of the exponentiation. Returns None if 'a' does not have a modular inverse.
        """
        ...

    def square(self, a: int) -> int:
        """
        Square an integer in the field.

        :param a: The integer to square.
        :return: The square of the integer.
        """
        ...

    def sqrt(self, a: int) -> int:
        """
        Compute the square root of an integer in the field.

        :param a: The integer to find the square root of.
        :return: The square root of the integer. Returns None if 'a' does not have a square root.
        """
        ...

    def mod_inv(self, a: int) -> int:
        """
        Compute the modular inverse of an integer in the field.

        :param a: The integer to find the modular inverse of.
        :return: The modular inverse of the integer. Returns None if 'a' does not have a modular inverse.
        """
        ...

    def neg(self, a: int) -> int:
        """
        Negate an integer in the field.

        :param a: The integer to negate.
        :return: The negation of the integer.
        """
        ...

    def has_sqrt(self, a: int) -> bool:
        """
        Check if an integer has a square root in the field.

        :param a: The integer to check.
        :return: True if the integer has a square root, False otherwise.
        """
        ...
