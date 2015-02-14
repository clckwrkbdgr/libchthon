#pragma once
#include "format.h"
#include "log.h"
#include <functional>
#include <algorithm>
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
 * Point is represented by a vector of coordinates of specified Size.
 * Coordinates are stored as an continuous array.
 */
template<class T, size_t Size>
struct BasePoint {
	T v[Size];
	/** Constructs point using specified coordinate values.
	 * If count of values is lesser than size of the vector, remaining values
	 * are filled using default constructors.
	 */
	template<typename ...Args>
	BasePoint(const T & value, Args ...args) : v{value, args...} {}
	/** Constructs null point. @see null() */
	BasePoint() : v() {}
	/** Constructs point using other point's values. */
	BasePoint(const BasePoint & other) : v()
	{
		std::copy(std::begin(other.v), std::end(other.v), std::begin(v));
	}
	/** Assigns other point value to this point. */
	BasePoint & operator=(const BasePoint & other)
	{
		std::copy(std::begin(other.v), std::end(other.v), std::begin(v));
		return *this;
	}
	/** Returns true if both coords are zeroes, otherwise returns false */
	bool null() const {
		auto is_zero = [](const T & value) { return equal(value, T()); };
		return std::all_of(std::begin(v), std::end(v), is_zero);
	}
	/** Accesses specified coordinate of the point. */
	T & operator[](int index) { return v[index]; }
	/** Accesses specified coordinate of the point. */
	const T & operator[](int index) const { return v[index]; }
	/** Appends other point to this point. */
	BasePoint & operator+=(const BasePoint & other)
	{
		std::transform(std::begin(v), std::end(v), std::begin(other.v),
				std::begin(v), std::plus<T>());
		return *this;
	}
	/** Subtracts other point from this point. */
	BasePoint & operator-=(const BasePoint & other)
	{
		std::transform(std::begin(v), std::end(v), std::begin(other.v),
				std::begin(v), std::minus<T>());
		return *this;
	}
	/** Multiplies point coordinates by factor. */
	template<class FactorType>
	BasePoint & operator*=(const FactorType & factor)
	{
		auto multiplies = [&factor](const T & value) { return T(value * factor); };
		std::transform(std::begin(v), std::end(v), std::begin(v),
				multiplies);
		return *this;
	}
	/** Divides point coordinates by factor. */
	template<class FactorType>
	BasePoint & operator/=(const FactorType & factor)
	{
		auto divides = [&factor](const T & value) { return T(value / factor); };
		std::transform(std::begin(v), std::end(v), std::begin(v),
				divides);
		return *this;
	}
};
/** Returns true if points are equal, otherwise returns false. */
template<class T, size_t Size>
bool operator==(const BasePoint<T, Size> & a, const BasePoint<T, Size> & b)
{
	auto current = std::begin(a.v);
	auto end = std::end(a.v);
	auto other = std::begin(b.v);
	for(; current != end; ++current, ++other) {
		if(!equal(*current, *other)) {
			return false;
		}
	}
	return true;
}
/** Returns true if points are not equal, otherwise returns false. */
template<class T, size_t Size>
bool operator!=(const BasePoint<T, Size> & a, const BasePoint<T, Size> & b)
{
	return !(a == b);
}
/** Returns true if a is lesser than b, otherwise returns false. */
template<class T, size_t Size>
bool operator<(const BasePoint<T, Size> & a, const BasePoint<T, Size> & b)
{
	auto current = std::begin(a.v);
	auto end = std::end(a.v);
	auto other = std::begin(b.v);
	for(; current != end; ++current, ++other) {
		if(!equal(*current, *other)) {
			return (*current < *other);
		}
	}
	return false;
}
/** Returns a sum of two points. */
template<class T, size_t Size>
BasePoint<T, Size> operator+(const BasePoint<T, Size> & a, const BasePoint<T, Size> & b)
{
	BasePoint<T, Size> result = a;
	return result += b;
}
/** Returns a diffrence between two points. */
template<class T, size_t Size>
BasePoint<T, Size> operator-(const BasePoint<T, Size> & a, const BasePoint<T, Size> & b)
{
	BasePoint<T, Size> result = a;
	return result -= b;
}
/** Returns point formed by multiplying both coordinates by given `factor`. */
template<class T, size_t Size, class FactorType>
BasePoint<T, Size> operator*(const BasePoint<T, Size> & a, const FactorType & factor)
{
	BasePoint<T, Size> result = a;
	return result *= factor;
}

/** Returns point formed by dividing both coordinates by given `factor`. */
template<class T, size_t Size, class FactorType>
BasePoint<T, Size> operator/(const BasePoint<T, Size> & a, const FactorType & factor)
{
	BasePoint<T, Size> result = a;
	return result /= factor;
}
/** Returns string in form: "(x, y, ...)", where `x`, `y`, `...` are point coordinates. */
template<class T, size_t Size>
std::string to_string(const BasePoint<T, Size> & value)
{
	std::string result = "(";
	bool first = true;
	for(const auto & item : value.v) {
		if(first) {
			first = false;
		} else {
			result += ", ";
		}
		result += std::to_string(item);
	}
	result += ")";
	return result;
}

/** Calculates length of the vector defined by specified point
 */
template<class T, size_t Size>
T length(const BasePoint<T, Size> & p)
{
	BasePoint<T, Size> tmp = p;
	std::transform(std::begin(tmp.v), std::end(tmp.v), std::begin(tmp.v),
			[](const T & value) { return value * value; }
			);
	return T(std::sqrt(std::accumulate(std::begin(tmp.v), std::end(tmp.v), T())));
}

/** Calculates distance between two points using Euclid metric.
 * Result is converted to point element type.
 */
template<class T, size_t Size>
T distance(const BasePoint<T, Size> & a, const BasePoint<T, Size> & b)
{
	return length(a - b);
}

/** Calculates dot product of the two given points (vectors).
 */
template<class T, size_t Size>
T dot_product(const BasePoint<T, Size> & a, const BasePoint<T, Size> & b)
{
	T result = T();
	auto a_it = std::begin(a.v);
	auto b_it = std::begin(b.v);
	for(; a_it != std::end(a.v); ++a_it, ++b_it) {
		result += *a_it * *b_it;
	}
	return result;
}

/** Returns normalized value of a given point.
 */
template<class T, size_t Size>
BasePoint<T, Size> normalize(const BasePoint<T, Size> & p)
{
	return p / length(p);
}

/** Calculates cross product of the two given 3D points (vectors).
 */
template<class T>
BasePoint<T, 3> cross_product(const BasePoint<T, 3> & u, const BasePoint<T, 3> & v)
{
	return BasePoint<T, 3>(u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]);
}

/** 2D point, subclassing base Point template.
 * Values can be accessed by convinience members `x` and `y`.
 */
template<class T>
struct Point2D : BasePoint<T, 2>
{
	typedef BasePoint<T, 2> base;
	T & x, & y;
	Point2D(const T & _x, const T & _y)
		: BasePoint<T, 2>{_x, _y}, x(base::v[0]), y(base::v[1]) {}
	Point2D()
		: BasePoint<T, 2>(), x(base::v[0]), y(base::v[1]) {}
	Point2D(const Point2D & other)
		: BasePoint<T, 2>(other), x(base::v[0]), y(base::v[1]) {}
	Point2D(const BasePoint<T, 2> & other)
		: BasePoint<T, 2>(other), x(base::v[0]), y(base::v[1]) {}
	Point2D & operator=(const Point2D & other)
	{
		BasePoint<T, 2>::operator=(other);
		return *this;
	}
};

/** 2-dimensional integer point.
 * Kept for compatibility and convinience.
 */
typedef Point2D<int> Point;

/** 3D point, subclassing base Point template.
 * Values can be accessed by convinience members `x`, `y`, `z`.
 */
template<class T>
struct Point3D : BasePoint<T, 3>
{
	typedef BasePoint<T, 2> base;
	T & x, & y, & z;
	Point3D(const T & _x, const T & _y, const T & _z)
		: BasePoint<T, 3>{_x, _y, _z}, x(base::v[0]), y(base::v[1]), z(base::v[2]) {}
	Point3D()
		: BasePoint<T, 3>(), x(base::v[0]), y(base::v[1]), z(base::v[2]) {}
	Point3D(const Point3D & other)
		: BasePoint<T, 3>(other), x(base::v[0]), y(base::v[1]), z(base::v[2]) {}
	Point3D(const BasePoint<T, 3> & other)
		: BasePoint<T, 3>(other), x(base::v[0]), y(base::v[1]), z(base::v[2]) {}
	Point3D & operator=(const Point3D & other)
	{
		BasePoint<T, 3>::operator=(other);
		return *this;
	}
};


/// @}
}

