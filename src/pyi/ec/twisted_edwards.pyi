"""
Type annotations for twisted Edwards curve operations.
"""
from typing import Optional, overload
from ..fp1 import Fp1
from .point import Point


class Curve:
    def __init__(self, field: Fp1, a: int, d: int) -> None:
        """
        Creates a twisted Edwards curve over the given field with parameters a and d.
        """
        ...

    def field(self) -> Fp1:
        """
        Returns the underlying finite field.
        """
        ...

    def a(self) -> int:
        """
        Returns the curve parameter a.
        """
        ...

    def d(self) -> int:
        """
        Returns the curve parameter d.
        """
        ...


class ExtPoint:
    def __init__(self, X: int = 0, Y: int = 1, Z: int = 1, T: int = 0) -> None:
        """
        Creates an extended twisted Edwards point with coordinates (X, Y, Z, T).
        """
        ...

    def X(self) -> int:
        ...

    def Y(self) -> int:
        ...

    def Z(self) -> int:
        ...

    def T(self) -> int:
        ...

    def __eq__(self, other: 'ExtPoint') -> bool:
        ...

    def __ne__(self, other: 'ExtPoint') -> bool:
        ...


def identity() -> Point:
    """
    Returns the affine identity point.
    """
    ...


def ext_identity() -> 'ExtPoint':
    """
    Returns the extended identity point.
    """
    ...


def to_ext(curve: Curve, p: Point) -> ExtPoint:
    """
    Converts an affine point to extended coordinates.
    """
    ...


def to_affine(curve: Curve, p: ExtPoint) -> Optional[Point]:
    """
    Converts an extended point to affine coordinates.
    """
    ...


@overload
def in_curve(curve: Curve, p: Point) -> bool:
    ...


@overload
def in_curve(curve: Curve, p: ExtPoint) -> bool:
    ...


def in_curve(curve: Curve, p: Point | ExtPoint) -> bool:
    """
    Checks whether a point lies on the curve.
    """
    ...


def equivalent(curve: Curve, a: ExtPoint, b: ExtPoint) -> bool:
    """
    Checks whether two extended points represent the same affine point.
    """
    ...


@overload
def add(curve: Curve, a: Point, b: Point) -> Point:
    ...


@overload
def add(curve: Curve, a: ExtPoint, b: ExtPoint) -> ExtPoint:
    ...


def add(curve: Curve, a: Point | ExtPoint, b: Point | ExtPoint) -> Point | ExtPoint:
    """
    Adds two points of the same representation.
    """
    ...


@overload
def dbl(curve: Curve, p: Point) -> Point:
    ...


@overload
def dbl(curve: Curve, p: ExtPoint) -> ExtPoint:
    ...


def dbl(curve: Curve, p: Point | ExtPoint) -> Point | ExtPoint:
    """
    Doubles a point.
    """
    ...


@overload
def mul_scalar(curve: Curve, p: Point, k: int) -> Point:
    ...


@overload
def mul_scalar(curve: Curve, p: ExtPoint, k: int) -> ExtPoint:
    ...


def mul_scalar(curve: Curve, p: Point | ExtPoint, k: int) -> Point | ExtPoint:
    """
    Multiplies a point by a scalar.
    """
    ...
