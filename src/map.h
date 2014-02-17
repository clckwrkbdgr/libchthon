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
	/// Constructs Map with specified size filled with filler.
	Map(unsigned map_width, unsigned map_height, const T & filler)
		: w(map_width), h(map_height), cells(w * h, filler) {}
	/// Constructs Map from sequence of values.
	/// Sequence should be exactly the size of W * H.
	template<class Iterator>
	Map(unsigned map_width, unsigned map_height, Iterator first, Iterator last)
		: w(map_width), h(map_height), cells(first, last) {}
	~Map() {}

	iterator begin() { return cells.begin(); }
	const_iterator begin() const { return cells.begin(); }
	iterator end() { return cells.begin() + w * h; }
	const_iterator end() const { return cells.begin() + w * h; }

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

	/// Run floodfill algorithm.
	/// Replaces all found values of which_value to to_what_value,
	/// starting with (x, y) point.
	void floodfill(int x, int y, const T & which_value, const T & to_what_value)
	{
		if(cell(x, y) != which_value) {
			return;
		}
		if(cell(x, y) == to_what_value) {
			return;
		}
		cell(x, y) = to_what_value;
		if(x > 0) {
			floodfill(x - 1, y, which_value, to_what_value);
		}
		if(x < width() - 1) {
			floodfill(x + 1, y, which_value, to_what_value);
		}
		if(y > 0) {
			floodfill(x, y - 1, which_value, to_what_value);
		}
		if(y < height() - 1) {
			floodfill(x, y + 1, which_value, to_what_value);
		}
	}
	/// Run floodfill algorithm (overloaded function).
	/// Replaces all found values of which_value to to_what_value,
	/// starting with pos point.
	void floodfill(const Point & pos, const T & which_value, const T & to_what_value)
	{
		floodfill(pos.x, pos.y, which_value, to_what_value);
	}
	/// Run floodfill algorithm (overloaded function).
	/// Replaces all found values of that under (x, y) to to_what_value,
	/// starting with (x, y) point.
	void floodfill(int x, int y, const T & to_what_value)
	{
		floodfill(x, y, cell(x, y), to_what_value);
	}
	/// Run floodfill algorithm (overloaded function).
	/// Replaces all found values of that under pos to to_what_value,
	/// starting with pos point.
	void floodfill(const Point & pos, const T & to_what_value)
	{
		floodfill(pos.x, pos.y, to_what_value);
	}

private:
	unsigned w, h;
	std::vector<T> cells;
};

/// @}
}

