#include "../src/fov.h"
#include "../src/map.h"
#include "../src/util.h"
#include "../src/test.h"

SUITE(fov) {

static const char data[] = 
	"# #"
	" #@"
	;

template<class T>
static void strip_fov_to_the_map(std::set<Chthon::Point> & fov, const Chthon::Map<T> & map)
{
	for(auto i = fov.begin(); i != fov.end();) {
		if(map.valid(*i)) {
			++i;
		} else {
			fov.erase(i++);
		}
	}
}

TEST(should_calculate_visible_area_within_sight_radius)
{
	Chthon::Map<char> map(3, 2);
	std::copy(std::begin(data), std::end(data), map.begin());
	std::set<Chthon::Point> fov = Chthon::get_fov(
			Chthon::Point(2, 1), 3,
			[map](const Chthon::Point & p) { return map.valid(p) && map.cell(p) != '#'; }
			);
	strip_fov_to_the_map(fov, map);
	EQUAL(fov.size(), 4);
	ASSERT(Chthon::contains(fov, Chthon::Point(1, 0)));
	ASSERT(Chthon::contains(fov, Chthon::Point(2, 0)));
	ASSERT(Chthon::contains(fov, Chthon::Point(1, 1)));
	ASSERT(Chthon::contains(fov, Chthon::Point(2, 1)));
}

TEST(should_not_see_through_opaque_cells)
{
	Chthon::Map<char> map(3, 2);
	std::copy(std::begin(data), std::end(data), map.begin());
	std::set<Chthon::Point> fov = Chthon::get_fov(
			Chthon::Point(2, 1), 3,
			[map](const Chthon::Point & p) { return map.valid(p) && map.cell(p) == ' '; }
			);
	ASSERT(!Chthon::contains(fov, Chthon::Point(0, 1)));
}

}
