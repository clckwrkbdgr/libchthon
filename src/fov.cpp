#include "fov.h"

namespace Chthon {

Ray::Ray(const Point & start, const Point & stop)
{
	int dx = std::abs(stop.x - start.x);
	int dy = std::abs(stop.y - start.y);
	int deltax = stop.x - start.x;
	int deltay = stop.y - start.y;
	error = 0.0;
	int iy = deltay > 0 ? 1 : -1;
	int ix = deltax > 0 ? 1 : -1;
	hor = dx > dy;
	delta_error = hor ? std::abs(double(deltay) / double(deltax))
		: std::abs(double(deltax) / double(deltay));
	side = hor ? start.y : start.x;
	i_side = hor ? iy : ix;
	pos = hor ? start.x : start.y;
	i_pos = hor ? ix : iy;
	end = hor ? stop.x : stop.y;
	current_point = start;
}

bool Ray::done() const
{
	return pos * i_pos >= end * i_pos;
}

const Point & Ray::current() const
{
	return current_point;
}

void Ray::to_next()
{
	error += delta_error;
	if(error > 0.5) {
		side += i_side;
		error -= 1.0;
	}
	pos += i_pos;
	if(hor) {
		current_point = Point(pos, side);
	} else {
		current_point = Point(side, pos);
	}
}

/// @cond INTERNAL
FOV::FOV(const Point & light_pos, int light_distance)
	: center(light_pos), radius(light_distance),
	current(center.x - radius, center.y - radius)
{
	fov.insert(center);
}

bool FOV::done() const
{
	return current == center;
}

void FOV::to_next()
{
	if(done()) {
		return;
	}
	++current.x;
	if(current == center) {
		++current.x;
	}
	if(current.x > center.x + radius) {
		current.x = 0;
		++current.y;
	}
	if(current.y > center.y + radius) {
		current = center;
	}
}

bool FOV::is_in_radius(const Point & p) const
{
	int dx = std::abs(p.x - center.x);
	int dy = std::abs(p.y - center.y);
	int distance = int(std::sqrt(dx * dx + dy * dy));
	return distance <= radius;
}
/// @endcond

}
