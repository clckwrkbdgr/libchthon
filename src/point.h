#pragma once
#include "format.h"
#include <vector>
#include <string>
#include <limits>
#include <cmath>

namespace Chthon { /// @defgroup Point Points and vectors
/// @{

/** Returns true if two objects are equal. */
template<class T>
bool equal(const T & a, const T & b)
{
	return a == b;
}
/** Returns true if two float point variables are considered equal. */
template<class T>
bool float_equal(const T & a, const T & b)
{
	return std::abs(a - b) < std::numeric_limits<T>::epsilon();
}
/** Returns true if two float point variables are considered equal. */
template<> bool equal(const float & a, const float & b);
/** Returns true if two float point variables are considered equal. */
template<> bool equal(const double & a, const double & b);
/** Returns true if two float point variables are considered equal. */
template<> bool equal(const long double & a, const long double & b);

/** Defines a point in the plane using integer precision.
 * Point is specified by a pair of coordinates: `x` and `y`.
 * Coordinates are stored as class members with public access.
 */
template<class T>
struct BasePoint {
	T x, y;
	/** Constructs point using _x and _y coordinates. */
	BasePoint(const T & _x, const T & _y) : x(_x), y(_y) {}
	/** Constructs null point. @see null() */
	BasePoint() : x(), y() {}
	/** Returns true if both coords are zeroes, otherwise returns false */
	bool null() const { return equal(x, T()) && equal(y, T()); }
	/** Appends other point to this point. */
	BasePoint & operator+=(const BasePoint & other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	/** Subtracts other point from this point. */
	BasePoint & operator-=(const BasePoint & other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}
	/** Multiplies point coordinates by factor. */
	template<class FactorType>
	BasePoint & operator*=(const FactorType & factor)
	{
		x = T(x * factor);
		y = T(y * factor);
		return *this;
	}
	/** Divides point coordinates by factor. */
	template<class FactorType>
	BasePoint & operator/=(const FactorType & factor)
	{
		x = T(x / factor);
		y = T(y / factor);
		return *this;
	}
};
typedef BasePoint<int> Point;

/** Returns true if points are equal, otherwise returns false. */
template<class T>
bool operator==(const BasePoint<T> & a, const BasePoint<T> & b)
{
	return equal(a.x, b.x) && equal(a.y, b.y);
}
/** Returns true if points are not equal, otherwise returns false. */
template<class T>
bool operator!=(const BasePoint<T> & a, const BasePoint<T> & b)
{
	return !(a == b);
}
/** Returns true if a is lesser than b, otherwise returns false. */
template<class T>
bool operator<(const BasePoint<T> & a, const BasePoint<T> & b)
{
	if(a.x == b.x) {
		return a.y < b.y;
	}
	return a.x < b.x;
}
/** Returns a sum of two points. */
template<class T>
BasePoint<T> operator+(const BasePoint<T> & a, const BasePoint<T> & b)
{
	BasePoint<T> result = a;
	return result += b;
}
/** Returns a diffrence between two points. */
template<class T>
BasePoint<T> operator-(const BasePoint<T> & a, const BasePoint<T> & b)
{
	BasePoint<T> result = a;
	return result -= b;
}
/** Returns point formed by multiplying both coordinates by given `factor`. */
template<class T, class FactorType>
BasePoint<T> operator*(const BasePoint<T> & a, const FactorType & factor)
{
	BasePoint<T> result = a;
	return result *= factor;
}

/** Returns point formed by dividing both coordinates by given `factor`. */
template<class T, class FactorType>
BasePoint<T> operator/(const BasePoint<T> & a, const FactorType & factor)
{
	BasePoint<T> result = a;
	return result /= factor;
}
/** Returns string in form: "(x, y)", where `x` and `y` are point coordinates. */
template<class T>
std::string to_string(const BasePoint<T> & value)
{
    return format("({0}, {1})", value.x, value.y);
}

/** Calculates distance between two points.
 * Formula is: `sqrt((a.x - b.x) ^ 2 + (a.y - b.y) ^ 2)`.
 * Result is converted to point element type.
 */
template<class T>
T distance(const BasePoint<T> & a, const BasePoint<T> & b)
{
	T x = a.x - b.x;
	T y = a.y - b.y;
	return T(std::sqrt(x * x + y * y));
}


/// @}
}
