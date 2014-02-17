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

static std::string get_fov_as_string(const std::set<Chthon::Point> & fov)
{
	Chthon::Map<char> fov_map(9, 9, ' ');
	for(const Chthon::Point & p : fov) {
		if(fov_map.valid(p)) {
			fov_map.cell(p) = '*';
		}
	}
	return std::string(fov_map.begin(), fov_map.end());
}

TEST(should_see_as_most_as_it_can)
{
	static const char data[] =
		"         "
		"         "
		"         "
		"         "
		"    @    "
		"         "
		"         "
		"         "
		"         "
		;
	Chthon::Map<char> map(9, 9, std::begin(data), std::end(data));

	std::set<Chthon::Point> fov = Chthon::get_fov(
			Chthon::Point(4, 4), 3,
			[map](const Chthon::Point & p) { return map.valid(p) && map.cell(p) != '#'; }
			);

	static const char expected[] =
		"         "
		"  *****  "
		" ******* "
		" ******* "
		" ******* "
		" ******* "
		" ******* "
		"  *****  "
		"         "
		;
	EQUAL(get_fov_as_string(fov), expected);
}

TEST(should_not_see_behind_walls)
{
	static const char data[] =
		"         "
		"         "
		"         "
		"    #    "
		"    @    "
		"         "
		"         "
		"         "
		"         "
		;
	Chthon::Map<char> map(9, 9, std::begin(data), std::end(data));

	std::set<Chthon::Point> fov = Chthon::get_fov(
			Chthon::Point(4, 4), 3,
			[map](const Chthon::Point & p) { return map.valid(p) && map.cell(p) != '#'; }
			);

	static const char expected[] =
		"         "
		"  *   *  "
		" **   ** "
		" ******* "
		" ******* "
		" ******* "
		" ******* "
		"  *****  "
		"         "
		;
	EQUAL(get_fov_as_string(fov), expected);
}

TEST(should_see_walls_themselves)
{
	static const char data[] =
		"         "
		"         "
		"  #####  "
		"  #   #  "
		"  # @ #  "
		"  #   #  "
		"  #####  "
		"         "
		"         "
		;
	Chthon::Map<char> map(9, 9, std::begin(data), std::end(data));

	std::set<Chthon::Point> fov = Chthon::get_fov(
			Chthon::Point(4, 4), 3,
			[map](const Chthon::Point & p) { return map.valid(p) && map.cell(p) != '#'; }
			);

	static const char expected[] =
		"         "
		"         "
		"  *****  "
		"  *****  "
		"  *****  "
		"  *****  "
		"  *****  "
		"         "
		"         "
		;
	EQUAL(get_fov_as_string(fov), expected);
}

}
