"""
This module provides type annotations for the Pedersen hash function.
"""
from typing import Union


def pedersen(preimage: Union[bytes, bytearray]) -> bytearray:
    """
    Compute the Pedersen hash of the given preimage.

    :param preimage: The byte array to hash.
    :return: The Pedersen hash of the preimage.
    """
    ...
