#include "level.h"
#include "game.h"
#include "objects.h"
#include "monsters.h"
#include "items.h"
#include "pathfinding.h"
#include "fov.h"
#include "log.h"
#include "format.h"
#include <cmath>
#include <algorithm>

namespace Chthon {

Level::Level()
	: map(1, 1)
{
}

Level::~Level()
{
}

Level::Level(unsigned map_width, unsigned map_height)
	: map(map_width, map_height)
{
}

const CellType & Level::cell_type_at(const Point & pos) const
{
	return deref_default(map.cell(pos).type);
}

const Monster & Level::get_player() const
{
	foreach(const Monster & monster, monsters) {
		if(deref_default(monster.type).faction == Monster::PLAYER) {
			return monster;
		}
	}
	static Monster empty;
	return empty;
}

Monster & Level::get_player()
{
	foreach( Monster & monster, monsters) {
		if(deref_default(monster.type).faction == Monster::PLAYER) {
			return monster;
		}
	}
	static Monster empty;
	empty = Monster();
	return empty;
}

CompiledInfo Level::get_info(int x, int y) const
{
	CompiledInfo result(Point(x, y));
	return result.in(monsters).in(items).in(objects).in(map);
}

CompiledInfo Level::get_info(const Point & pos) const
{
	return get_info(pos.x, pos.y);
}

void Level::invalidate_fov(Monster & monster)
{
	for(int x = 0; x < int(map.width()); ++x) {
		for(int y = 0; y < int(map.height()); ++y) {
			map.cell(x, y).visible = false;
		}
	}
	std::set<Point> visible_points = get_fov(
			monster.pos, deref_default(monster.type).sight,
			[this](const Point & p) { return get_info(p).compiled().transparent; }
			);
	for(const Point & p : visible_points) {
		if(!map.valid(p)) {
			continue;
		}
		map.cell(p).visible = true;
		if(deref_default(monster.type).faction == Monster::PLAYER) {
			map.cell(p).seen_sprite = get_info(p).compiled().sprite;
		}
	}
}

std::list<Point> Level::find_path(const Point & player_pos, const Point & target)
{
	Pathfinder pathfinder;
	pathfinder.lee(player_pos, target,
			[this](const Point & pos) {
				return get_info(pos).compiled().passable;
			});
	return pathfinder.directions;
}

void Level::erase_dead_monsters()
{
	monsters.erase(std::remove_if(monsters.begin(), monsters.end(), std::mem_fun_ref(&Monster::is_dead)), monsters.end());
}


void DungeonBuilder::fill_room(Map<Cell> & map, const std::pair<Point, Point> & room, const CellType * type)
{
	for(int x = room.first.x; x <= room.second.x; ++x) {
		for(int y = room.first.y; y <= room.second.y; ++y) {
			map.cell(x, y) = Cell(type);
		}
	}
}

std::vector<Point> DungeonBuilder::random_positions(const std::pair<Point, Point> & room, unsigned count)
{
	std::vector<Point> result;
	for(unsigned i = 0; i < count; ++i) {
		int width = (room.second.x - room.first.x);
		int height = (room.second.y - room.first.y);
		int counter = width * height;
		while(--counter > 0) {
			int x = rand() % width + room.first.x;
			int y = rand() % height + room.first.y;
			if(result.end() == std::find(result.begin(), result.end(), Point(x, y))) {
				result << Point(x, y);
				break;
			}
		}
	}
	for(unsigned i = result.size(); i < count; ++i) {
		result << room.first;
	}
	return result;
}

std::pair<Point, Point> DungeonBuilder::connect_rooms(Level & level, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, const CellType * type)
{
	if(a.first.x < b.first.x) {
		int start_y = std::max(a.first.y, b.first.y);
		int stop_y = std::min(a.second.y, b.second.y);
		int way = start_y + rand() % (stop_y - start_y);
		for(int x = a.second.x + 1; x != b.first.x; ++x) {
			level.map.cell(x, way) = Cell(type);
		}
		return std::make_pair(Point(a.second.x + 1, way), Point(b.first.x - 1, way));
	}
	if(a.first.x > b.first.x) {
		int start_y = std::max(a.first.y, b.first.y);
		int stop_y = std::min(a.second.y, b.second.y);
		int way = start_y + rand() % (stop_y - start_y);
		for(int x = b.second.x + 1; x != a.first.x; ++x) {
			level.map.cell(x, way) = Cell(type);
		}
		return std::make_pair(Point(b.second.x + 1, way), Point(a.first.x - 1, way));
	}
	if(a.first.y < b.first.y) {
		int start_x = std::max(a.first.x, b.first.x);
		int stop_x = std::min(a.second.x, b.second.x);
		int wax = start_x + rand() % (stop_x - start_x);
		for(int y = a.second.y + 1; y != b.first.y; ++y) {
			level.map.cell(wax, y) = Cell(type);
		}
		return std::make_pair(Point(wax, a.second.y + 1), Point(wax, b.first.y - 1));
	}
	if(a.first.y > b.first.y) {
		int start_x = std::max(a.first.x, b.first.x);
		int stop_x = std::min(a.second.x, b.second.x);
		int wax = start_x + rand() % (stop_x - start_x);
		for(int y = b.second.y + 1; y != a.first.y; ++y) {
			level.map.cell(wax, y) = Cell(type);
		}
		return std::make_pair(Point(wax, b.second.y + 1), Point(wax, a.first.y - 1));
	}
	return std::make_pair(Point(), Point());
}

std::vector<std::pair<Point, Point> > DungeonBuilder::shuffle_rooms(const std::vector<std::pair<Point, Point> > & rooms)
{
	static unsigned a00[] = { 8, 1, 2, 7, 0, 3, 6, 5, 4, };
	static unsigned a01[] = { 6, 7, 8, 5, 0, 1, 4, 3, 2, };
	static unsigned a02[] = { 4, 5, 6, 3, 0, 7, 2, 1, 8, };
	static unsigned a03[] = { 2, 3, 4, 1, 0, 5, 8, 7, 6, };
	static unsigned a04[] = { 2, 1, 8, 3, 0, 7, 4, 5, 6, };
	static unsigned a05[] = { 8, 7, 6, 1, 0, 5, 2, 3, 4, };
	static unsigned a06[] = { 6, 5, 4, 7, 0, 3, 8, 1, 2, };
	static unsigned a07[] = { 4, 3, 2, 5, 0, 1, 6, 7, 8, };
	static unsigned a08[] = { 0, 1, 2, 5, 4, 3, 6, 7, 8, };
	static unsigned a09[] = { 0, 1, 2, 7, 6, 3, 8, 5, 4, };
	static unsigned a10[] = { 0, 1, 2, 7, 8, 3, 6, 5, 4, };
	static unsigned a11[] = { 0, 5, 6, 1, 4, 7, 2, 3, 8, };
	static unsigned a12[] = { 0, 7, 8, 1, 6, 5, 2, 3, 4, };
	static unsigned a13[] = { 0, 7, 6, 1, 8, 5, 2, 3, 4, };
	static unsigned a14[] = { 6, 7, 8, 5, 4, 3, 0, 1, 2, };
	static unsigned a15[] = { 8, 5, 4, 7, 6, 3, 0, 1, 2, };
	static unsigned a16[] = { 6, 5, 4, 7, 8, 3, 0, 1, 2, };
	static unsigned a17[] = { 2, 3, 8, 1, 4, 7, 0, 5, 6, };
	static unsigned a18[] = { 2, 3, 4, 1, 6, 5, 0, 7, 8, };
	static unsigned a19[] = { 2, 3, 4, 1, 8, 5, 0, 7, 6, };
	static unsigned a20[] = { 2, 1, 0, 3, 4, 5, 8, 7, 6, };
	static unsigned a21[] = { 2, 1, 0, 3, 6, 7, 4, 5, 8, };
	static unsigned a22[] = { 2, 1, 0, 3, 8, 7, 4, 5, 6, };
	static unsigned a23[] = { 6, 5, 0, 7, 4, 1, 8, 3, 2, };
	static unsigned a24[] = { 8, 7, 0, 5, 6, 1, 4, 3, 2, };
	static unsigned a25[] = { 6, 7, 0, 5, 8, 1, 4, 3, 2, };
	static unsigned a26[] = { 8, 7, 6, 3, 4, 5, 2, 1, 0, };
	static unsigned a27[] = { 4, 5, 8, 3, 6, 7, 2, 1, 0, };
	static unsigned a28[] = { 4, 5, 6, 3, 8, 7, 2, 1, 0, };
	static unsigned a29[] = { 8, 3, 2, 7, 4, 1, 6, 5, 0, };
	static unsigned a30[] = { 4, 3, 2, 5, 6, 1, 8, 7, 0, };
	static unsigned a31[] = { 4, 3, 2, 5, 8, 1, 6, 7, 0, };
	static unsigned * layouts[] = {
		a00, a01, a02, a03, a04, a05, a06, a07,
		a08, a09, a10, a11, a12, a13, a14, a15,
		a16, a17, a18, a19, a20, a21, a22, a23,
		a24, a25, a26, a27, a28, a29, a30, a31,
	};
	unsigned * a = layouts[rand() % 32];

	std::vector<std::pair<Point, Point> > new_rooms(9);
	for(unsigned i = 0; i < rooms.size(); ++i) {
		new_rooms[a[i]] = rooms[i];
	}
	return new_rooms;
}

void DungeonBuilder::pop_player_front(std::vector<Monster> & monsters)
{
	if(monsters.empty()) {
		return;
	}
	foreach(Monster & monster, monsters) {
		if(deref_default(monster.type).faction == Monster::PLAYER) {
			std::swap(monster, monsters.front());
			log("Player found.");
			break;
		}
	}
}


}
