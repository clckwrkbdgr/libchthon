#include "point.h"
#include "format.h"
#include <cmath>

namespace Chthon {

Point::Point(int _x, int _y)
	: x(_x), y(_y)
{
}

Point::Point()
	: x(0), y(0)
{
}

bool Point::null() const
{
	return x == 0 && y == 0;
}

Point & Point::operator+=(const Point & other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Point & Point::operator-=(const Point & other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Point & Point::operator*=(int factor)
{
	x *= factor;
	y *= factor;
	return *this;
}

Point & Point::operator/=(int factor)
{
	x /= factor;
	y /= factor;
	return *this;
}

bool operator==(const Point & a, const Point & b)
{
	return a.x == b.x && a.y == b.y;
}

bool operator!=(const Point & a, const Point & b)
{
	return !(a == b);
}

Point operator+(const Point & a, const Point & b)
{
	Point result = a;
	return result += b;
}

Point operator-(const Point & a, const Point & b)
{
	Point result = a;
	return result -= b;
}

Point operator*(const Point & a, int factor)
{
	Point result = a;
	return result *= factor;
}

Point operator/(const Point & a, int factor)
{
	Point result = a;
	return result /= factor;
}

std::string to_string(const Point & value)
{
    return format("({0}, {1})", value.x, value.y);
}

int distance(const Point & a, const Point & b)
{
	int x = a.x - b.x;
	int y = a.y - b.y;
	return int(std::sqrt(x * x + y * y));
}

}
