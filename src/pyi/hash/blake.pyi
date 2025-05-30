"""
This module provides type annotations for the BLAKE hash functions.
"""
from typing import Union


def blake256(bytes: Union[bytes, bytearray]) -> bytearray:
    """
    Compute BLAKE256 hash of a byte array.

    :param bytes: The byte array to hash.
    :return: The BLAKE256 hash of the byte array.
    """
    ...
