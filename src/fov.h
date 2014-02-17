#pragma once
#include "point.h"
#include <set>
#include <cmath>

namespace Chthon { /// @defgroup FOV Field-of-vision
/// @{

/** Class for ray casting. Traverses between start and stop points
 * using Bresenham algorithm.
 */
class Ray {
public:
	/// Constructs ray from start to stop point.
	Ray(const Point & start, const Point & stop);
	/// Returns true, is current point is past stop, otherwise return false.
	bool done() const;
	/// Returns current point.
	const Point & current() const;
	/// Moves to next point.
	void to_next();
private:
	bool hor;
	double error, delta_error;
	int side, i_side, pos, i_pos, end;
	Point current_point;
};

/** Calculates visible points, starting from light_pos and with max distance of
 * light_distance. Visibility of each point is calculated using is_transparent
 * function object:
 * @code{.cpp}
 * bool is_transparent(const Point & p);
 * @endcode
 * Result is a set of points. Some of points could not be valid map points, as
 * routine does not check any boundaries, only distance is taken into consideration.
 * First opaque cell in the way of a ray is also considered visible.
 */
template<class IsTransparent>
std::set<Point> get_fov(const Point & light_pos, int light_distance, IsTransparent is_transparent)
{
	std::set<Point> result;
	for(int x = light_pos.x - light_distance; x <= light_pos.x + light_distance; ++x) {
		for(int y = light_pos.y - light_distance; y <= light_pos.y + light_distance; ++y) {
			int dx = std::abs(x - light_pos.x);
			int dy = std::abs(y - light_pos.y);
			int distance = int(std::sqrt(dx * dx + dy * dy));
			bool can_see = distance <= light_distance;
			if(!can_see) {
				continue;
			}
			Ray ray(light_pos, Point(x, y));
			while(!ray.done() && is_transparent(ray.current())) {
				ray.to_next();
			}
			if(ray.done()) {
				result.insert(Point(x, y));
			}
		}
	}
	return result;
}

/// @}
}
