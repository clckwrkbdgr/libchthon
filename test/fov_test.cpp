#include "../src/fov.h"
#include "../src/map.h"
#include "../src/util.h"
#include "../src/test.h"

SUITE(fov) {

static std::string get_fov_as_string(const std::set<Chthon::Point> & fov)
{
	Chthon::Map<char> fov_map(9, 9);
	std::fill(fov_map.begin(), fov_map.end(), ' ');
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
	Chthon::Map<char> map(9, 9);
	std::copy(std::begin(data), std::end(data), map.begin());

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
	Chthon::Map<char> map(9, 9);
	std::copy(std::begin(data), std::end(data), map.begin());

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
	Chthon::Map<char> map(9, 9);
	std::copy(std::begin(data), std::end(data), map.begin());

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
