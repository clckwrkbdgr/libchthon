#pragma once
#include "point.h"
#include <vector>

namespace Chthon {

template<class T>
class Map {
public:
	unsigned width, height;
	std::vector<T> cells;

	Map(unsigned map_width, unsigned map_height)
		: width(map_width), height(map_height), cells(width * height, T()) {}
	~Map() {}

	bool valid(int x, int y) const
	{
		return (x >= 0 && x < int(width) && y >= 0 && y < int(height));
	}
	bool valid(const Point & pos) const { return valid(pos.x, pos.y); }

	const T & cell(int x, int y) const
	{
		static T empty;
		if(!valid(x, y)) {
			return empty;
		}
		return cells[x + y * width];
	}
	const T & cell(const Point & pos) const { return cell(pos.x, pos.y); }
	T & cell(int x, int y)
	{
		static T empty;
		empty = T();
		if(!valid(x, y)) {
			return empty;
		}
		return cells[x + y * width];
	}
	T & cell(const Point & pos) { return cell(pos.x, pos.y); }

	void fill(const T & value)
	{
		std::fill(cells.begin(), cells.end(), value);
	}
	void fill(const T * map_of_values)
	{
		std::copy(map_of_values, map_of_values + width * height, cells.begin());
	}
};

}
