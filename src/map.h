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
	Map(unsigned map_width = 0, unsigned map_height = 0)
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
		return (0 <= x && x < int(w) && 0 <= y && y < int(h));
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

	void resize(unsigned new_width, unsigned new_height)
	{
		typename std::vector<T> new_pixels(new_width * new_height, 0);
		typename std::vector<T>::const_iterator src = cells.begin();
		typename std::vector<T>::iterator dest = new_pixels.begin();
		unsigned min_common_width = std::min(w, new_width);
		unsigned current_col = 0;
		unsigned rows_left_to_copy = std::min(h, new_height);
		int src_row_inc = w > new_width ? int(w) - int(new_width) : 0;
		int dest_row_inc = w < new_width ? int(new_width) - int(w) : 0;
		while(rows_left_to_copy) {
			*dest++ = *src++;
			++current_col;
			if(current_col == min_common_width) {
				--rows_left_to_copy;
				if(!rows_left_to_copy) {
					break;
				}
				src += src_row_inc;
				dest += dest_row_inc;
				current_col = 0;
			}
		}
		cells.swap(new_pixels);
		w = new_width;
		h = new_height;
	}
private:
	unsigned w, h;
	std::vector<T> cells;
};

/// @}
}
