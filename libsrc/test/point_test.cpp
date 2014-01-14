#include "../point.h"
#include "../util.h"
#include "../test.h"

using namespace Chthon::UnitTest;

namespace Chthon {

SUITE(point) {

TEST(should_make_point_from_two_coords)
{
	Point p(1, 2);
	EQUAL(p.x, 1);
	EQUAL(p.y, 2);
}

TEST(should_consider_non_zero_point_non_null)
{
	Point p(1, 2);
	ASSERT(!p.null());
}

TEST(should_consider_zero_point_null)
{
	Point p(0, 0);
	ASSERT(p.null());
}

TEST(should_make_point_from_point)
{
	Point p(1, 2);
	Point d = p;
	EQUAL(p.x, d.x);
	EQUAL(p.y, d.y);
}

TEST(should_add_point_to_point)
{
	Point a(1, 2), b(2, 5);
	Point c = a + b;
	EQUAL(c.x, 3);
	EQUAL(c.y, 7);
}

TEST(should_subtract_point_from_point)
{
	Point a(1, 2), b(2, 5);
	Point c = a - b;
	EQUAL(c.x, -1);
	EQUAL(c.y, -3);
}

TEST(should_divide_point_by_factor)
{
	Point a(16, 25);
	Point c = a / 4;
	EQUAL(c.x, 4);
	EQUAL(c.y, 6);
}

TEST(should_multiply_point_by_factor)
{
	Point a(4, 6);
	Point c = a * 4;
	EQUAL(c.x, 16);
	EQUAL(c.y, 24);
}

TEST(should_convert_point_to_string)
{
	std::string result = to_string(Point(10, 0));
	EQUAL(result, "(10, 0)");
}

struct PosInt {
	int value;
	Point pos;
	PosInt() : value(0) {}
	PosInt(int _value, const Point & _pos) : value(_value), pos(_pos) {}
};

TEST(should_find_item_in_vector)
{
	std::vector<PosInt> v; v << PosInt(1, Point(1, 0)) << PosInt(2, Point(0, 1)) << PosInt(3, Point(0, 0));
	PosInt & i = find_at(v, Point(0, 1));
	EQUAL(i.value, 2);
}

TEST(should_change_found_item_in_vector)
{
	std::vector<PosInt> v; v << PosInt(1, Point(1, 0)) << PosInt(2, Point(0, 1)) << PosInt(3, Point(0, 0));
	PosInt & i = find_at(v, Point(0, 1));
	i.value = 4;
	EQUAL(v[1].value, 4);
}

TEST(should_return_empty_item_if_not_found_in_vector)
{
	std::vector<PosInt> v; v << PosInt(1, Point(1, 0)) << PosInt(2, Point(0, 1)) << PosInt(3, Point(1, 1));
	PosInt & i = find_at(v, Point(2, 2));
	EQUAL(i.value, 0);
	ASSERT(i.pos.null());
}

TEST(should_find_item_in_const_vector)
{
	std::vector<PosInt> a; a << PosInt(1, Point(1, 0)) << PosInt(2, Point(0, 1)) << PosInt(3, Point(0, 0));
	const std::vector<PosInt> & v = a;
	PosInt i = find_at(v, Point(0, 1));
	EQUAL(i.value, 2);
}

TEST(should_return_empty_item_if_not_found_in_const_vector)
{
	std::vector<PosInt> a; a << PosInt(1, Point(1, 0)) << PosInt(2, Point(0, 1)) << PosInt(3, Point(1, 1));
	const std::vector<PosInt> & v = a;
	PosInt i = find_at(v, Point(2, 2));
	EQUAL(i.value, 0);
	ASSERT(i.pos.null());
}


TEST(should_compute_distance_between_points)
{
	Point a(1, 2), b(11, 12);
	int d = distance(a, b);
	EQUAL(d, 14);
}

TEST(should_compute_distance_between_points_on_x_axis)
{
	Point a(1, 3), b(1, 13);
	int d = distance(a, b);
	EQUAL(d, 10);
}

TEST(should_compute_distance_between_points_on_y_axis)
{
	Point a(1, 3), b(13, 3);
	int d = distance(a, b);
	EQUAL(d, 12);
}

TEST(should_return_zero_for_same_points)
{
	Point a(1, 3), b(1, 3);
	int d = distance(a, b);
	EQUAL(d, 0);
}

TEST(should_return_one_for_close_point)
{
	Point a(1, 3), b(2, 4);
	int d = distance(a, b);
	EQUAL(d, 1);
}

}

}
