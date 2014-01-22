#pragma once
#include "point.h"
#include <vector>

namespace Chthon { /// @defgroup Map Map
/// @{

/** Represents 2d map of cells.
 */
template<class T>
class Map {
public:
	unsigned width, height;
	std::vector<T> cells;

	/// Constructs Map with specified size. All values is set to default.
	Map(unsigned map_width, unsigned map_height)
		: width(map_width), height(map_height), cells(width * height, T()) {}
	~Map() {}

	/// Checks if point (x, y) is valid, i.e. inside the map bounds.
	bool valid(int x, int y) const
	{
		return (x >= 0 && x < int(width) && y >= 0 && y < int(height));
	}
	/// Checks if point (x, y) is valid, i.e. inside the map bounds.
	bool valid(const Point & pos) const { return valid(pos.x, pos.y); }

	/// Returns cell at the specified position if position is valid, otherwise returns default value.
	const T & cell(int x, int y) const
	{
		static T empty;
		if(!valid(x, y)) {
			return empty;
		}
		return cells[x + y * width];
	}
	/// Returns cell at the specified position if position is valid, otherwise returns default value.
	const T & cell(const Point & pos) const { return cell(pos.x, pos.y); }
	/// Returns cell at the specified position if position is valid, otherwise returns default value.
	T & cell(int x, int y)
	{
		static T empty;
		empty = T();
		if(!valid(x, y)) {
			return empty;
		}
		return cells[x + y * width];
	}
	/// Returns cell at the specified position if position is valid, otherwise returns default value.
	T & cell(const Point & pos) { return cell(pos.x, pos.y); }

	/// Fills map with specified value.
	void fill(const T & value)
	{
		std::fill(cells.begin(), cells.end(), value);
	}
	/// Fills map using array of values. Array's size must be at least (width * height).
	void fill(const T * map_of_values)
	{
		std::copy(map_of_values, map_of_values + width * height, cells.begin());
	}
};

/// @}
}
