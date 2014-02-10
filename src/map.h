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
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;

	/// Constructs Map with specified size.
	/// All values is constructed using default constructor.
	Map(unsigned map_width, unsigned map_height)
		: w(map_width), h(map_height), cells(w * h, T()) {}
	~Map() {}

	iterator begin() { return cells.begin(); }
	const_iterator begin() const { return cells.begin(); }
	iterator end() { return cells.end(); }
	const_iterator end() const { return cells.end(); }

	unsigned width() const { return w; }
	unsigned height() const { return h; }

	/// Returns pointer to the first item.
	const T * data() const { return &cells.front(); }
	/// Returns pointer to the first item.
	T * data() { return &cells.front(); }

	/// Checks if point (x, y) is valid, i.e. inside the map bounds.
	bool valid(int x, int y) const
	{
		return (x >= 0 && x < int(w) && y >= 0 && y < int(h));
	}
	/// Checks if point (x, y) is valid, i.e. inside the map bounds.
	bool valid(const Point & pos) const { return valid(pos.x, pos.y); }

	/// Returns cell at the specified position.
	/// If position is invalid, behaviour is undefined;
	const T & cell(int x, int y) const
	{
		return cells[x + y * w];
	}
	/// Returns cell at the specified position.
	/// If position is invalid, behaviour is undefined;
	const T & cell(const Point & pos) const { return cell(pos.x, pos.y); }
	/// Returns cell at the specified position.
	/// If position is invalid, behaviour is undefined;
	T & cell(int x, int y)
	{
		return cells[x + y * w];
	}
	/// Returns cell at the specified position.
	/// If position is invalid, behaviour is undefined;
	T & cell(const Point & pos) { return cell(pos.x, pos.y); }
private:
	unsigned w, h;
	std::vector<T> cells;
};

/// @}
}
