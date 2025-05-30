"""
This module provides type annotations for the BitVec1D class.
"""

class BitVec1D:
    """
    A class representing a one-dimensional bit vector.
    """

    def __init__(self) -> None:
        """
        Initializes an empty BitVec1D.
        """
        ...

    def size(self) -> int:
        """
        Get the number of bits in the vector.

        :return: The number of bits.
        """
        ...

    def push(self, bit: bool) -> None:
        """
        Add a bit to the end of the vector.

        :param bit: The bit to add.
        """
        ...

    def __getitem__(self, index: int) -> bool:
        """
        Get the bit at the specified index.

        :param index: The index of the bit to retrieve.
        :return: The bit at the specified index.
        """
        ...

    def __len__(self) -> int:
        """
        Get the number of bits in the vector.

        :return: The number of bits.
        """
        ...
