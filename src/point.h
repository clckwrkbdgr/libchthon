#pragma once
#include <vector>
#include <string>

namespace Chthon { /// @defgroup Point Points
/// @{

/** Defines a point in the plane using integer precision.
 * Point is specified by a pair of coordinates: `x` and `y`.
 * Coordinates are stored as class members with public access.
 */
struct Point {
	int x, y;
	/** Constructs point using _x and _y coordinates. */
	Point(int _x, int _y);
	/** Constructs null point. @see null() */
	Point();
	/** Returns true if both coords are zeroes, otherwise returns false */
	bool null() const;
	/** Appends other point to this point. */
	Point & operator+=(const Point & other);
	/** Subtracts other point from this point. */
	Point & operator-=(const Point & other);
	/** Multiplies point coordinates by factor. */
	Point & operator*=(int factor);
	/** Divides point coordinates by factor. */
	Point & operator/=(int factor);
};
/** Returns true if points are equal, otherwise returns false. */
bool operator==(const Point & a, const Point & b);
/** Returns true if points are not equal, otherwise returns false. */
bool operator!=(const Point & a, const Point & b);
/** Returns true if a is lesser than b, otherwise returns false. */
bool operator<(const Point & a, const Point & b);
/** Returns a sum of two points. */
Point operator+(const Point & a, const Point & b);
/** Returns a diffrence between two points. */
Point operator-(const Point & a, const Point & b);
/** Returns point formed by multiplying both coordinates by given `factor`. */
Point operator*(const Point & a, int factor);
/** Returns point formed by dividing both coordinates by given `factor`. */
Point operator/(const Point & a, int factor);
/** Returns string in form: "(x, y)", where `x` and `y` are point coordinates. */
std::string to_string(const Point & value);

/** Calculates distance between two points.
 * Formula is: `sqrt((a.x - b.x) ^ 2 + (a.y - b.y) ^ 2)`.
 * Result is converted to integer precision.
 */
int distance(const Point & a, const Point & b);

/// @}
}
