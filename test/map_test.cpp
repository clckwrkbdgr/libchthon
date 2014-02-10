#include "../src/map.h"
#include "../src/point.h"
#include "../src/test.h"
using Chthon::Map;

SUITE(map) {

TEST(point_inside_map_should_be_valid)
{
	Map<int> map(3, 3);
	Chthon::Point point(1, 1);
	ASSERT(map.valid(point));
}

TEST(point_outside_map_should_be_invalid)
{
	Map<int> map(3, 3);
	Chthon::Point point(-1, 1);
	ASSERT(!map.valid(point));
}

TEST(should_use_iterators)
{
	Map<int> map(2, 2);
	std::fill(map.begin(), map.end(), 1);
	EQUAL(map.cell(0, 0), 1);
	EQUAL(map.cell(1, 0), 1);
	EQUAL(map.cell(0, 1), 1);
	EQUAL(map.cell(1, 1), 1);
}

TEST(should_fill_map_with_multi_values)
{
	Map<int> map(2, 2);
	int a[] = {1, 2, 3, 4};
	std::copy(a, a + 4, map.begin());
	EQUAL(map.cell(0, 0), 1);
	EQUAL(map.cell(1, 0), 2);
	EQUAL(map.cell(0, 1), 3);
	EQUAL(map.cell(1, 1), 4);
}

}
