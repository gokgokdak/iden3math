class Point:
    def __init__(self, x: int = 0, y: int = 0) -> None:
        """
        Creates a point with given x and y coordinates.
        
        :param x: The x coordinate of the point.
        :param y: The y coordinate of the point.
        """
        ...

    def x(self) -> int:
        """
        Returns the x coordinate of the point.
        
        :return: The x coordinate.
        """
        ...

    def y(self) -> int:
        """
        Returns the y coordinate of the point.
        
        :return: The y coordinate.
        """
        ...

    def __eq__(self, other: 'Point') -> bool:
        """
        Checks if two points are equal.
        
        :param other: The other point to compare.
        :return: True if the points are equal, False otherwise.
        """
        ...

    def __ne__(self, other: 'Point') -> bool:
        """
        Checks if two points are not equal.
        
        :param other: The other point to compare.
        :return: True if the x and y coordinates are not equal, False otherwise.
        """
        ...
