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

/** Searches vector for an object at specified position and returns found object.
 * If no such object is found, then returns a default contstructed value.
 * Container values must have `pos` member of Point type.
 * If index is specified, it holds pointer to iterator which links to returned object.
 */
template<class T>
const T & find_at(const std::vector<T> & container, const Point & pos, typename std::vector<T>::const_iterator * index = nullptr)
{
	typename std::vector<T>::const_iterator i;
    for(i = container.begin(); i != container.end(); ++i) {
        if(i->pos == pos) {
			if(index) {
				*index = i;
			}
            return *i;
        }
    }
    static T empty;
    return empty;
}

/** Searches vector for an object at specified position and returns found object.
 * If no such object is found, then returns a default contstructed value.
 * Container values must have `pos` member of Point type.
 * If index is specified, it holds pointer to iterator which links to returned object.
 */
template<class T>
T & find_at(std::vector<T> & container, const Point & pos, typename std::vector<T>::iterator * index = nullptr)
{
	typename std::vector<T>::iterator i;
    for(i = container.begin(); i != container.end(); ++i) {
        if(i->pos == pos) {
			if(index) {
				*index = i;
			}
            return *i;
        }
    }
    static T empty;
	empty = T();
    return empty;
}

/** Calculates distance between two points.
 * Formula is: `sqrt((a.x - b.x) ^ 2 + (a.y - b.y) ^ 2)`.
 * Result is converted to integer precision.
 */
int distance(const Point & a, const Point & b);

/// @}
}
