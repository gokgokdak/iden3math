"""
This module provides type annotations for the MiMC hash functions.
"""
from typing import Union


def mimc_sponge(preimages: list[Union[bytes, bytearray]],
                outputs: int,
                key: Union[bytes, bytearray],
                preimage_endian: 'Endian' = 'Endian'.BE,
                key_endian: 'Endian' = 'Endian'.BE,
                digest_endian: 'Endian' = 'Endian'.BE) -> list[bytearray]:
    """
    Compute MiMC sponge hash for multiple preimages with an optional key.

    :param preimages: The 2D byte array of preimages to hash.
    :param outputs: The number of output hashes to generate.
    :param key: The byte array key for the hash function.
    :param preimage_endian: The endian format of the preimages (default is BE).
    :param key_endian: The endian format of the key (default is BE).
    :param digest_endian: The endian format of the output digest (default is BE).
    :return: The MiMC sponge hash of the preimages.
    """
    ...
