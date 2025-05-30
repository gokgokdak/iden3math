"""
This module provides type annotations for the random module.
"""
from typing import Union


def get_bytes(len: int) -> bytearray:
    """
    Generate a random byte vector of the specified length.

    :param len: The length of the byte vector to generate.
    :return: A random byte vector of the specified length.
    """
    ...

def get_integer(p: int) -> int:
    """
    Generate a random integer in the range [1, p).

    :param p: The upper bound of the range (exclusive).
    :return: A random integer in the range [1, p).
    """
    ...
