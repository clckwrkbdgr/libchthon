#pragma once
#include <list>
namespace Chthon { class Point; }

namespace Chthon { /// @defgroup Pathfinding Pathfinding algorithms
/// @{

/** Simple Lee pathfinding algorithm.
 * In order to use subclass it and define is_passable() protected method.
 */
class LeeAlgorithm {
public:
	/// Storeas best found path if exists, empty path otherwise.
	std::list<Point> best_path;

	virtual ~LeeAlgorithm();

	/// Runs pathfinding between points start and target (including them).
	/// Returns true if path was found, otherwise returns false.
	/// Found path is stored in best_path member.
	bool find_path(const Point & start, const Point & target);
protected:
	/// Should return true, if pos is passable, otherwise false.
	virtual bool is_passable(const Point & pos) const = 0;
};

/// @}
}
