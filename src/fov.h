#pragma once
#include "point.h"
#include "log.h"
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

/// @cond INTERNAL
struct FOV {
	std::set<Point> fov;

	FOV(const Point & light_pos, int light_distance);
	bool done() const;
	template<class IsTransparent>
	void check_current(IsTransparent is_transparent)
	{
		if(!is_in_radius(current)) {
			return;
		}

		Ray ray(center, current);
		while(!ray.done() && is_transparent(ray.current())) {
			ray.to_next();
		}
		if(ray.done()) {
			fov.insert(current);
		}
	}
	void to_next();
private:
	bool is_in_radius(const Point & p) const;
	const Point & center;
	int radius;
	Point current;
};
/// @endcond

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
	FOV fov(light_pos, light_distance);
	while(!fov.done()) {
		fov.check_current(is_transparent);
		fov.to_next();
	}
	return fov.fov;
}

/// @}
}
