#include "pathfinding.h"

namespace Chthon {

const Point LeeAlgorithmImpl::shifts[] = {
	Point(-1, -1), Point(0, -1), Point(1, -1),
	Point(-1, 0), Point(1, 0),
	Point(-1, 1), Point(0, 1), Point(1, 1),
};

LeeAlgorithmImpl::LeeAlgorithmImpl(const Point & target)
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

}

