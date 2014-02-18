#include "../src/fov.h"
#include "../src/map.h"
#include "../src/util.h"
#include "../src/log.h"
#include "../src/test.h"

namespace Chthon {

template<class T>
bool operator!=(const Chthon::Map<T> & a, const Chthon::Map<T> & b)
{
	return !std::equal(a.begin(), a.end(), b.begin());
}

static std::string to_string(const Chthon::Map<char> & map)
{
	std::string result;
	unsigned count = 0;;
	for(char c : map) {
		if(count-- == 0) {
			result += "\n";
			count = map.width() - 1;
		}
		result += c;
	}
	result += "\n";
	return result;
}

}

SUITE(fov) {

struct RayData {
	Chthon::Map<char> map, expected_map;
	std::string expected;

	RayData() : map(9, 9, ' '), expected_map(9, 9, ' ') {}
};

TEST(should_cast_ray)
{
	static const std::string char_data[] = {
		"#        ","  #      ","    #    ","      #  ","        #","         ",
		" #       ","   #     ","    #    ","     #   ","       # ","         ",
		"  #      ","   #     ","    #    ","     #   ","      #  ","        #",
		"   #     ","    #    ","    #    ","    #    ","     #   ","      ## ",
		"    #    ","    #    ","    #    ","    #    ","    #    ","    ##   ",
		"         ","         ","         ","         ","         ","         ",
		"         ","         ","         ","         ","         ","         ",
		"         ","         ","         ","         ","         ","         ",
		"         ","         ","         ","         ","         ","         ",
		""
	};
	static const Chthon::Point dest[] = {
		{0, 0}, {2, 0}, {4, 0}, {6, 0}, {8, 0}, {8, 2},
	};
	enum { DIAGONAL, LEFT, STRAIGHT, RIGHT, LEFT_DIAGONAL, LEFT_UP, COUNT };
	RayData data[COUNT];
	for(const std::string * line = char_data; !line->empty(); ) {
		for(int i = 0; i < COUNT; ++i) {
			data[i].expected += *line++;
		}
	}

	for(int i = 0; i < COUNT; ++i) {
		std::copy(data[i].expected.begin(), data[i].expected.end(), data[i].expected_map.begin());
		Chthon::Ray ray(Chthon::Point(4, 4), dest[i]);
		while(!ray.done()) {
			data[i].map.cell(ray.current()) = '#';
			ray.to_next();
		}
		data[i].map.cell(ray.current()) = '#';
	}
	EQUAL(data[DIAGONAL].map, data[DIAGONAL].expected_map);
	EQUAL(data[LEFT].map, data[LEFT].expected_map);
	EQUAL(data[STRAIGHT].map, data[STRAIGHT].expected_map);
	EQUAL(data[RIGHT].map, data[RIGHT].expected_map);
}

template<class Iterator>
std::vector<std::string> from_side_by_side(Iterator first, Iterator last, unsigned count)
{
	std::vector<std::string> result(count);
	unsigned current = 0;
	for(Iterator it = first; it != last; ++it) {
		if(current >= count) {
			current = 0;
		}
		result[current] += *it;
		++current;
	}
	return result;
}

static const std::string map_data[] = {
	"         ","         ","         ",
	"         ","         ","         ",
	"         ","         ","  #####  ",
	"         ","    #    ","  #   #  ",
	"    @    ","    @    ","  # @ #  ",
	"         ","         ","  #   #  ",
	"         ","         ","  #####  ",
	"         ","         ","         ",
	"         ","         ","         ",
};
static const std::vector<std::string> maps =
	from_side_by_side(std::begin(map_data), std::end(map_data), 3);
static const std::string expected_data[] = {
	"         ","         ","         ",
	"  *****  ","  *   *  ","         ",
	" ******* "," **   ** ","  *****  ",
	" ******* "," ******* ","  *****  ",
	" ******* "," ******* ","  *****  ",
	" ******* "," ******* ","  *****  ",
	" ******* "," ******* ","  *****  ",
	"  *****  ","  *****  ","         ",
	"         ","         ","         ",
};
static const std::vector<std::string> ex =
	from_side_by_side(std::begin(expected_data), std::end(expected_data), 3);

struct Data {
	Chthon::Map<char> map, expected;
	Data() {}
	Data(unsigned w, unsigned h,
			const std::string & map_string, const std::string & expected_string)
		: map(w, h, map_string.begin(), map_string.end()),
		expected(w, h, expected_string.begin(), expected_string.end()) {}
};

TEST_DATA(fov, Data(9, 9, maps[0], ex[0]), should_see_as_most_as_it_can);
TEST_DATA(fov, Data(9, 9, maps[1], ex[1]), should_not_see_behind_walls);
TEST_DATA(fov, Data(9, 9, maps[2], ex[2]), should_see_walls_themselves)
{
	std::set<Chthon::Point> fov_points = Chthon::get_fov(
			Chthon::Point(4, 4), 3,
			[fov](const Chthon::Point & p) { return fov.map.valid(p) && fov.map.cell(p) != '#'; }
			);
	Chthon::Map<char> fov_map(9, 9, ' ');
	for(const Chthon::Point & p : fov_points) {
		if(fov_map.valid(p)) {
			fov_map.cell(p) = '*';
		}
	}

	EQUAL(fov_map, fov.expected);
}

}
