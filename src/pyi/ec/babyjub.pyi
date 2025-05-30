"""
This module provides type annotations for the BabyJubjub elliptic curve operations.
"""
from typing import Union


def prime() -> int:
    """
    Returns the prime number of the BabyJubjub curve.
    
    :return: The prime number.
    """
    ...

def finite_field() -> int:
    """
    Returns the finite field of the BabyJubjub curve.
    
    :return: The finite field.
    """
    ...

def group_order() -> int:
    """
    Returns the order of the group of points on the BabyJubjub curve.
    
    :return: The group order.
    """
    ...

def sub_group_order() -> int:
    """
    Returns the order of the subgroup of points on the BabyJubjub curve.
    
    :return: The subgroup order.
    """
    ...

def zero() -> 'Point':
    """
    Returns the zero point (identity element) of the BabyJubjub curve.
    
    :return: The zero point.
    """
    ...

def generator() -> 'Point':
    """
    Returns the generator point of the BabyJubjub curve.
    
    :return: The generator point.
    """
    ...

def add(a: 'Point', b: 'Point') -> 'Point':
    """
    Adds two points on the BabyJubjub curve.
    
    :param a: The first point.
    :param b: The second point.
    :return: The resulting point after addition.
    """
    ...

def mul_scalar(p: 'Point', k: int) -> 'Point':
    """
    Multiplies a point on the BabyJubjub curve by a scalar.
    
    :param p: The point to multiply.
    :param k: The scalar value.
    :return: The resulting point after multiplication.
    """
    ...

def in_sub_group(p: 'Point') -> bool:
    """
    Checks if a point is in the subgroup of the BabyJubjub curve.
    
    :param p: The point to check.
    :return: True if the point is in the subgroup, False otherwise.
    """
    ...

def in_curve(p: 'Point') -> bool:
    """
    Checks if a point is on the BabyJubjub curve.
    
    :param p: The point to check.
    :return: True if the point is on the curve, False otherwise.
    """
    ...

def compress(p: 'Point', endian: 'Endian') -> bytearray:
    """
    Compresses a point on the BabyJubjub curve into a byte vector.
    
    :param p: The point to compress.
    :param endian: The endianness to use for compression.
    :return: The compressed byte vector.
    """
    ...

def decompress(packed: Union[bytes, bytearray], endian: 'Endian') -> 'Point':
    """
    Decompresses a byte vector into a point on the BabyJubjub curve.
    
    :param packed: The byte vector to decompress.
    :param endian: The endianness used in the byte vector.
    :return: The decompressed point.
    """
    ...

