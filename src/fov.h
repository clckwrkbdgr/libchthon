#pragma once
#include "point.h"
#include <set>

namespace Chthon { /// @defgroup FOV Field-of-vision
/// @{

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
			if(can_see) {
				int deltax = x - light_pos.x;
				int deltay = y - light_pos.y;
				double error = 0.0;
				int iy = deltay > 0 ? 1 : -1;
				int ix = deltax > 0 ? 1 : -1;
				if(dx > dy) {
					double delta_error = std::abs(double(deltay) / double(deltax));
					int cy = light_pos.y;
					for(int cx = light_pos.x; cx != x; cx += ix) {
						if(!is_transparent(Point(cx, cy))) {
							can_see = false;
							break;
						}

						error += delta_error;
						if(error > 0.5) {
							cy += iy;
							error -= 1.0;
						}
					}
				} else {
					double delta_error = std::abs(double(deltax) / double(deltay));
					int cx = light_pos.x;
					for(int cy = light_pos.y; cy != y; cy += iy) {
						if(!is_transparent(Point(cx, cy))) {
							can_see = false;
							break;
						}

						error += delta_error;
						if(error > 0.5) {
							cx += ix;
							error -= 1.0;
						}
					}
				}
			}
			if(can_see) {
				result.insert(Point(x, y));
			}
		}
	}
	return result;
}

/// @}
}
