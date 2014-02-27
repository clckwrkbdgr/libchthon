#pragma once
#include "point.h"
#include <algorithm>
#include <list>
#include <set>

namespace Chthon { /// @defgroup Pathfinding Pathfinding algorithms
/// @{

/// @cond INTERNAL
struct LeeAlgorithmImpl {
	enum { MAX_WAVE_COUNT = 2000 };
	typedef std::set<Point> Wave;
	std::list<Wave> waves;
	Wave neighs;
	const std::vector<Point> shifts;

	LeeAlgorithmImpl(const Point & target, bool is_diagonal_movement_permitted = true);
	void new_wave();
	void add_new_neigh(const Point & neigh);
	void construct_path(const Point & start, std::list<Point> & path, std::list<Point> & directions);

	template<class IsPassable>
	bool produce_waves(const Point & start, IsPassable is_passable)
	{
		while(waves.size() < MAX_WAVE_COUNT) {
			new_wave();
			for(const Point & point : waves.front()) {
				for(const Point & shift : shifts) {
					Point n = point + shift;
					if(!is_passable(n)) {
						continue;
					}
					if(n == start) {
						return true;
					}
					add_new_neigh(n);
				}
			}
		}
		return false;
	}
};
/// @endcond

/** Incapsulates pathfinding.
 *
 * Usage:
 *
 * @code{.cpp}
 * Pathfinder finder;
 * finder.lee(start, taget, is_passable);
 * // Do smth with finder.path or finder.directions...
 * @endcode
 */
class Pathfinder {
public:
	/// Stores found path if exists, empty path otherwise.
	std::list<Point> path;
	/** Stores directions needed to traverse found path.
	 * I.e. for path `(0, 0)-(1, 0)-(2, 1)` directions will be
	 * `(1, 0)-(1-1)`.
	 */
	std::list<Point> directions;

	/// Permits/disables diagonal movement.
	bool diagonal_movement_permitted;

	/// Constructs pathfinder object with specified options.
	Pathfinder(bool is_diagonal_movement_permitted = true);

	/** Simple Lee pathfinding algorithm.
	 * Runs pathfinding between points start and target (including them).
	 * Returns true if path was found, otherwise returns false.
	 * Uses is_passable function object to determine whether specified point
	 * is passable or not.
	 * is_passable() should take Point as an argument and return boolean value.
	 * Found path is stored in best_path member.
	 *
	 * **Warning**: argument of is_passable() could be an invalid point (i.e.
	 * a point out of map bounds)!
	 */
	template<class IsPassable>
	bool lee(const Point & start, const Point & target, IsPassable is_passable)
	{
		path.clear();
		directions.clear();
		if(!is_passable(target) || start == target) {
			return false;
		}

		LeeAlgorithmImpl impl(target, diagonal_movement_permitted);
		if(!impl.produce_waves(start, is_passable)) {
			return false;
		}
		impl.construct_path(start, path, directions);
		return true;
	}
};

/// @}
}
