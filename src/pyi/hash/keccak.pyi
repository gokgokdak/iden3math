"""
This module provides type annotations for the Keccak hash functions.
"""
from typing import Union


def keccak256(bytes: Union[bytes, bytearray]) -> bytearray:
    """
    Compute Keccak256 hash of a byte array.

    :param bytes: The byte array to hash.
    :return: The Keccak256 hash of the byte array.
    """
    ...
