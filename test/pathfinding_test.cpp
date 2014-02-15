#include "../src/pathfinding.h"
#include "../src/map.h"
#include "../src/test.h"
using Chthon::Point;

SUITE(pathfinding) {

static const char data[] =
	" #  "
	"#  #"
	" ## "
	" # #"
	;

TEST(should_find_path_between_points)
{
	Chthon::Map<char> map(4, 4);
	std::copy(std::begin(data), std::end(data), map.begin());
	Chthon::Pathfinder finder;
	bool ok = finder.lee(Point(0, 3), Point(2, 3),
			[map](const Point & pos) { return map.cell(pos) == ' '; }
			);
	ASSERT(ok);
	TEST_CONTAINER(finder.best_path, pos) {
		EQUAL(pos, Point(0, -1));
	} NEXT(pos) {
		EQUAL(pos, Point(1, -1));
	} NEXT(pos) {
		EQUAL(pos, Point(1,  0));
	} NEXT(pos) {
		EQUAL(pos, Point(1,  1));
	} NEXT(pos) {
		EQUAL(pos, Point(-1, 1));
	} DONE(pos);
}

TEST(should_find_path_between_close_points)
{
	Chthon::Map<char> map(4, 4);
	std::copy(std::begin(data), std::end(data), map.begin());
	Chthon::Pathfinder finder;
	bool ok = finder.lee(Point(0, 3), Point(0, 2),
			[map](const Point & pos) { return map.cell(pos) == ' '; }
			);
	ASSERT(ok);
	TEST_CONTAINER(finder.best_path, pos) {
		EQUAL(pos, Point(0, -1));
	} DONE(pos);
}

TEST(should_not_find_path_if_target_is_the_same_as_start)
{
	Chthon::Map<char> map(4, 4);
	std::copy(std::begin(data), std::end(data), map.begin());
	Chthon::Pathfinder finder;
	bool ok = finder.lee(Point(0, 3), Point(0, 3),
			[map](const Point & pos) { return map.cell(pos) == ' '; }
			);
	ASSERT(!ok);
}

}
