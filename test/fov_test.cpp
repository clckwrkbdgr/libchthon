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

static Chthon::InterleavedCharMap rays = {9, 9, 6, {
	"#        ","  #      ","    #    ","      #  ","        #","         ",
	" #       ","   #     ","    #    ","     #   ","       # ","         ",
	"  #      ","   #     ","    #    ","     #   ","      #  ","        #",
	"   #     ","    #    ","    #    ","    #    ","     #   ","      ## ",
	"    #    ","    #    ","    #    ","    #    ","    #    ","    ##   ",
	"         ","         ","         ","         ","         ","         ",
	"         ","         ","         ","         ","         ","         ",
	"         ","         ","         ","         ","         ","         ",
	"         ","         ","         ","         ","         ","         ",
}};

TEST_DATA(ray, Chthon::Point(0, 0), rays.value(0), should_cast_ray_diagonally);
TEST_DATA(ray, Chthon::Point(2, 0), rays.value(1), should_cast_ray_a_bit_left);
TEST_DATA(ray, Chthon::Point(4, 0), rays.value(2), should_cast_ray_straight);
TEST_DATA(ray, Chthon::Point(6, 0), rays.value(3), should_cast_ray_a_bit_right);
TEST_DATA(ray, Chthon::Point(8, 0), rays.value(4), should_cast_ray_diagonally_left);
TEST_DATA(ray, Chthon::Point(8, 2), rays.value(5), should_cast_ray_a_bit_down)
{
	Chthon::Map<char> map(9, 9, ' ');
	Chthon::Ray ray(Chthon::Point(4, 4), ray_data);
	while(!ray.done()) {
		map.cell(ray.current()) = '#';
		ray.to_next();
	}
	map.cell(ray.current()) = '#';
	Chthon::Map<char> expected(9, 9, ray_expected.begin(), ray_expected.end());
	EQUAL(map, expected);
}

static Chthon::InterleavedCharMap maps = {9, 9, 3, {
	"         ","         ","         ",
	"         ","         ","         ",
	"         ","         ","  #####  ",
	"         ","    #    ","  #   #  ",
	"    @    ","    @    ","  # @ #  ",
	"         ","         ","  #   #  ",
	"         ","         ","  #####  ",
	"         ","         ","         ",
	"         ","         ","         ",
}};
static Chthon::InterleavedCharMap expected_maps = {9, 9, 3, {
	"         ","         ","         ",
	"  *****  ","  *   *  ","         ",
	" ******* "," **   ** ","  *****  ",
	" ******* "," ******* ","  *****  ",
	" ******* "," ******* ","  *****  ",
	" ******* "," ******* ","  *****  ",
	" ******* "," ******* ","  *****  ",
	"  *****  ","  *****  ","         ",
	"         ","         ","         ",
}};

TEST_DATA(fov, maps.value(0), expected_maps.value(0), should_see_as_most_as_it_can);
TEST_DATA(fov, maps.value(1), expected_maps.value(1), should_not_see_behind_walls);
TEST_DATA(fov, maps.value(2), expected_maps.value(2), should_see_walls_themselves)
{
	Chthon::Map<char> map(9, 9, fov_data.begin(), fov_data.end());

	std::set<Chthon::Point> fov_points = Chthon::get_fov(
			Chthon::Point(4, 4), 3,
			[map](const Chthon::Point & p) { return map.valid(p) && map.cell(p) != '#'; }
			);
	Chthon::Map<char> fov_map(9, 9, ' ');
	for(const Chthon::Point & p : fov_points) {
		if(fov_map.valid(p)) {
			fov_map.cell(p) = '*';
		}
	}

	Chthon::Map<char> expected(9, 9, fov_expected.begin(), fov_expected.end());
	EQUAL(fov_map, expected);
}

}
