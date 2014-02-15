#include "pathfinding.h"
#include "point.h"
#include <algorithm>

namespace Chthon {

LeeAlgorithm::~LeeAlgorithm()
{
}

bool LeeAlgorithm::find_path(const Point & start, const Point & target)
{
	best_path.clear();
	if(!is_passable(target) || start == target) {
		return false;
	}

	std::vector<Point> shifts;
	for(Point shift(-1, 0); shift.x <= 1; ++shift.x) {
		for(shift.y = -1; shift.y <= 1; ++shift.y) {
			if(!shift.null()) {
				shifts.push_back(shift);
			}
		}
	}

	std::list<std::vector<Point> > waves;
	waves.push_front(std::vector<Point>(1, target));

	bool found = false;
	for(int i = 0; i < 2000; ++i) {
		std::vector<Point> neighs;
		for(const Point & point : waves.front()) {
			for(const Point & shift : shifts) {
				Point n = point + shift;
				if(n == start) {
					found = true;
					break;
				}
				if(!is_passable(n)) {
					continue;
				}
				bool already_present = false;
				for(const std::vector<Point> & wave : waves) {
					if(std::find(wave.begin(), wave.end(), n) != wave.end()) {
						already_present = true;
						break;
					}
				}
				if(!already_present) {
					if(std::find(neighs.begin(), neighs.end(), n) == neighs.end()) {
						neighs.push_back(n);
					}
				}
			}
			if(found) {
				break;
			}
		}
		if(found) {
			break;
		}
		waves.push_front(neighs);
	}

	Point prev = start;
	for(const std::vector<Point> wave : waves) {
		for(const Point & point : wave) {
			Point shift = Point(point.x - prev.x, point.y - prev.y);
			bool is_close = std::abs(shift.x) <= 1 && std::abs(shift.y) <= 1;
			if(!shift.null() && is_close) {
				prev = point;
				best_path.push_back(shift);
				break;
			}
		}
	}
	return found;
}

}
