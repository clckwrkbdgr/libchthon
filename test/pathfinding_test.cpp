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

class MapPath : public Chthon::LeeAlgorithm {
public:
	MapPath(const Chthon::Map<char> & _map) : map(_map) {}
	virtual ~MapPath() {}
protected:
	virtual bool is_passable(const Chthon::Point & pos) const { return map.cell(pos) == ' '; }
private:
	const Chthon::Map<char> & map;
};

TEST_FIXTURE(MapWithPath, should_find_path_between_points)
{
	MapPath path(map);
	bool ok = path.find_path(Point(0, 3), Point(2, 3));
	ASSERT(ok);
	TEST_CONTAINER(path.best_path, pos) {
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
	MapPath path(map);
	bool ok = path.find_path(Point(0, 3), Point(0, 2));
	ASSERT(ok);
	TEST_CONTAINER(path.best_path, pos) {
		EQUAL(pos, Point(0, -1));
	} DONE(pos);
}

TEST_FIXTURE(MapWithPath, should_not_find_path_if_target_is_the_same_as_start)
{
	MapPath path(map);
	bool ok = path.find_path(Point(0, 3), Point(0, 3));
	ASSERT(!ok);
}

}
