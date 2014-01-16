#pragma once
#include <vector>
#include <string>

namespace Chthon {

struct Point {
	int x, y;
	Point(int _x, int _y);
	Point();
	bool null() const;
	bool operator==(const Point & other) const;
	bool operator!=(const Point & other) const { return !(*this == other); }
	Point & operator+=(const Point & other);
	Point & operator-=(const Point & other);
	Point & operator*=(int factor);
	Point & operator/=(int factor);
};
Point operator+(const Point & a, const Point & b);
Point operator-(const Point & a, const Point & b);
Point operator*(const Point & a, int factor);
Point operator/(const Point & a, int factor);
std::string to_string(const Point & value);

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

int distance(const Point & a, const Point & b);

}
