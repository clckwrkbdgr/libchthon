#include "../src/pathfinding.h"
#include "../src/map.h"
#include "../src/test.h"
using Chthon::Point;

SUITE(pathfinding) {

struct MapWithPath {
	Chthon::Map<char> map;
	MapWithPath()
		: map(4, 4)
	{
		static const char data[] =
			" #  "
			"#  #"
			" ## "
			" # #"
			;
		std::copy(std::begin(data), std::end(data), map.begin());
	}
};

class MapPath {
public:
	MapPath(const Chthon::Map<char> & _map) : map(_map) {}
	bool operator()(const Chthon::Point & pos) const { return map.cell(pos) == ' '; }
private:
	const Chthon::Map<char> & map;
};

TEST_FIXTURE(MapWithPath, should_find_path_between_points)
{
	Chthon::Pathfinder finder;
	MapPath path(map);
	bool ok = finder.lee(Point(0, 3), Point(2, 3), path);
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

TEST_FIXTURE(MapWithPath, should_find_path_between_close_points)
{
	Chthon::Pathfinder finder;
	MapPath path(map);
	bool ok = finder.lee(Point(0, 3), Point(0, 2), path);
	ASSERT(ok);
	TEST_CONTAINER(finder.best_path, pos) {
		EQUAL(pos, Point(0, -1));
	} DONE(pos);
}

TEST_FIXTURE(MapWithPath, should_not_find_path_if_target_is_the_same_as_start)
{
	Chthon::Pathfinder finder;
	MapPath path(map);
	bool ok = finder.lee(Point(0, 3), Point(0, 3), path);
	ASSERT(!ok);
}

}
