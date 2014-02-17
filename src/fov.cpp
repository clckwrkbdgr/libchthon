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

}
