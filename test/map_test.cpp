#include "../src/map.h"
#include "../src/point.h"
#include "../src/test.h"
using Chthon::Map;

SUITE(map) {

TEST(should_create_map_filled_with_value)
{
	Map<char> map(3, 3, '#');
	std::string map_as_string(map.begin(), map.end());
	EQUAL(map_as_string, "#########");
}

TEST(should_create_map_from_sequence)
{
	static const char data[] =
		"# #"
		"@#@"
		"# #"
		;
	Map<char> map(3, 3, std::begin(data), std::end(data));
	std::string map_as_string(map.begin(), map.end());
	EQUAL(map_as_string, "# #@#@# #");
}

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

TEST(should_resize_map)
{
	Map<int> map(2, 2);
	map.resize(4, 4);
	EQUAL(map.width(), 4u);
	EQUAL(map.height(), 4u);
}

TEST(should_keep_existing_values_when_resizing_map)
{
	Map<int> map(2, 2);
	int a[] = {0, 1, 2, 3};
	std::copy(a, a + 4, map.begin());
	map.resize(4, 4);
	EQUAL(map.cell(0, 0), 0u);
	EQUAL(map.cell(1, 0), 1u);
	EQUAL(map.cell(0, 1), 2u);
	EQUAL(map.cell(1, 1), 3u);
}

TEST(should_fill_new_cells_with_default_values_when_resizing_map)
{
	Map<int> map(2, 2);
	std::fill(map.begin(), map.end(), 1);
	map.resize(2, 3);
	EQUAL(map.cell(0, 2), 0u);
	EQUAL(map.cell(1, 2), 0u);
}

TEST(should_flood_fill_map)
{
	std::string data = 
		"        "
		"  ###   "
		" #   #  "
		"  #  #  "
		"   # #  "
		"  #   # "
		" #    # "
		" #    # "
		;
	std::string expected_result = 
		"        "
		"  ###   "
		" #...#  "
		"  #..#  "
		"   #.#  "
		"  #...# "
		" #....# "
		" #....# "
		;
	Map<char> map(8, 8, data.begin(), data.end());

	map.floodfill(5, 5, ' ', '.');

	std::string result(64, ' ');
	std::copy(map.begin(), map.end(), result.begin());
	EQUAL(result, expected_result);
}

}
