#include "pathfinding.h"

namespace Chthon {

/// @cond INTERNAL
const Point all_shifts[] = {
	Point(-1, -1), Point(0, -1), Point(1, -1),
	Point(-1, 0), Point(1, 0),
	Point(-1, 1), Point(0, 1), Point(1, 1),
};

const Point straight_only_shifts[] = {
	Point(0, -1),
	Point(-1, 0), Point(1, 0),
	Point(0, 1),
};

LeeAlgorithmImpl::LeeAlgorithmImpl(const Point & target, bool is_diagonal_movement_permitted)
	: shifts(is_diagonal_movement_permitted ? std::begin(all_shifts) : std::begin(straight_only_shifts),
			is_diagonal_movement_permitted ? std::end(all_shifts): std::end(straight_only_shifts)
			)
{
	neighs.insert(target);
}

void LeeAlgorithmImpl::new_wave()
{
	waves.push_front(neighs);
	neighs.clear();
}

void LeeAlgorithmImpl::add_new_neigh(const Point & neigh)
{
	bool already_present = false;
	for(const Wave & wave : waves) {
		if(wave.count(neigh) > 0) {
			already_present = true;
			break;
		}
	}
	if(!already_present) {
		neighs.insert(neigh);
	}
}

void LeeAlgorithmImpl::construct_path(const Point & start, std::list<Point> & path, std::list<Point> & directions)
{
	path.push_back(start);
	for(const Wave &  wave : waves) {
		for(const Point & point : wave) {
			Point shift = Point(point.x - path.back().x, point.y - path.back().y);
			bool is_close = std::abs(shift.x) <= 1 && std::abs(shift.y) <= 1;
			if(!shift.null() && is_close) {
				path.push_back(point);
				directions.push_back(shift);
				break;
			}
		}
	}
}
/// @endcond


Pathfinder::Pathfinder(bool is_diagonal_movement_permitted)
	: diagonal_movement_permitted(is_diagonal_movement_permitted)
{
}

}

