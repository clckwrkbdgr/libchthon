#include "../src/point.h"
#include "../src/util.h"
#include "../src/test.h"
using Chthon::Point;

SUITE(point) {

TEST(should_consider_two_float_point_number_equal)
{
	ASSERT(Chthon::equal(3.1, 3.1));
}

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
	Point p2;
	ASSERT(p2.null());
}

TEST(should_assign_one_point_to_another)
{
	Point a(2, 3);
	Point b;
	b = a;
	EQUAL(a.x, b.x);
	EQUAL(a.y, b.y);
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
