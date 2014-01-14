#include "level.h"
#include "game.h"
#include "objects.h"
#include "monsters.h"
#include "items.h"
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

Level::Level(int map_width, int map_height)
	: map(map_width, map_height)
{
}

const CellType & Level::cell_type_at(const Point & pos) const
{
	return *(map.cell(pos).type);
}

const Monster & Level::get_player() const
{
	foreach(const Monster & monster, monsters) {
		if(monster.type->faction == Monster::PLAYER) {
			return monster;
		}
	}
	static Monster empty;
	return empty;
}

Monster & Level::get_player()
{
	foreach( Monster & monster, monsters) {
		if(monster.type->faction == Monster::PLAYER) {
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
	for(unsigned x = 0; x < map.width; ++x) {
		for(unsigned y = 0; y < map.height; ++y) {
			map.cell(x, y).visible = false;
		}
	}
	for(int x = monster.pos.x - monster.type->sight; x <= monster.pos.x + monster.type->sight; ++x) {
		for(int y = monster.pos.y - monster.type->sight; y <= monster.pos.y + monster.type->sight; ++y) {
			if(!map.valid(x, y)) {
				continue;
			}
			int dx = std::abs(x - monster.pos.x);
			int dy = std::abs(y - monster.pos.y);
			int distance = int(std::sqrt(dx * dx + dy * dy));
			bool can_see = distance <= monster.type->sight;
			if(can_see) {
				int deltax = x - monster.pos.x;
				int deltay = y - monster.pos.y;
				double error = 0.0;
				int iy = deltay > 0 ? 1 : -1;
				int ix = deltax > 0 ? 1 : -1;
				if(dx > dy) {
					double delta_error = std::abs(double(deltay) / double(deltax));
					int cy = monster.pos.y;
					for(int cx = monster.pos.x; cx != x; cx += ix) {
						if(!get_info(cx, cy).compiled().transparent) {
							can_see = false;
							break;
						}

						error += delta_error;
						if(error > 0.5) {
							cy += iy;
							error -= 1.0;
						}
					}
				} else {
					double delta_error = std::abs(double(deltax) / double(deltay));
					int cx = monster.pos.x;
					for(int cy = monster.pos.y; cy != y; cy += iy) {
						if(!get_info(cx, cy).compiled().transparent) {
							can_see = false;
							break;
						}

						error += delta_error;
						if(error > 0.5) {
							cx += ix;
							error -= 1.0;
						}
					}
				}
			}
			map.cell(x, y).visible = can_see;
			if(can_see && monster.type->faction == Monster::PLAYER) {
				map.cell(x, y).seen_sprite = get_info(x, y).compiled().sprite;
			}
		}
	}
}

std::list<Point> Level::find_path(const Point & player_pos, const Point & target)
{
	std::list<Point> best_path;
	if(!get_info(target.x, target.y).compiled().passable || player_pos == target) {
		return best_path;
	}

	std::vector<Point> shifts;
	for(Point shift(-1, 0); shift.x <= 1; ++shift.x) {
		for(shift.y = -1; shift.y <= 1; ++shift.y) {
			if(!shift.null()) {
				shifts.push_back(shift);
			}
		}
	}

	std::list<std::vector<Point> > waves;
	waves.push_front(std::vector<Point>(1, target));

	for(int i = 0; i < 2000; ++i) {
		bool found = false;
		std::vector<Point> neighs;
		foreach(const Point & point, waves.front()) {
			foreach(const Point & shift, shifts) {
				Point n = point + shift;
				if(n == player_pos) {
					found = true;
					break;
				}
				if(!get_info(n.x, n.y).compiled().passable) {
					continue;
				}
				bool already_present = false;
				foreach(const std::vector<Point> & wave, waves) {
					if(std::find(wave.begin(), wave.end(), n) != wave.end()) {
						already_present = true;
						break;
					}
				}
				if(!already_present) {
					if(std::find(neighs.begin(), neighs.end(), n) == neighs.end()) {
						neighs.push_back(n);
					}
				}
			}
			if(found) {
				break;
			}
		}
		if(found) {
			break;
		}
		waves.push_front(neighs);
	}

	Point prev = player_pos;
	foreach(const std::vector<Point> wave, waves) {
		foreach(const Point & point, wave) {
			Point shift = Point(point.x - prev.x, point.y - prev.y);
			bool is_close = std::abs(shift.x) <= 1 && std::abs(shift.y) <= 1;
			if(!shift.null() && is_close) {
				prev = point;
				best_path.push_back(shift);
				break;
			}
		}
	}
	return best_path;
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

std::vector<Point> DungeonBuilder::random_positions(const std::pair<Point, Point> & room, int count)
{
	std::vector<Point> result;
	for(int i = 0; i < count; ++i) {
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
	for(int i = result.size(); i < count; ++i) {
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
	static int a00[] = { 8, 1, 2, 7, 0, 3, 6, 5, 4, };
	static int a01[] = { 6, 7, 8, 5, 0, 1, 4, 3, 2, };
	static int a02[] = { 4, 5, 6, 3, 0, 7, 2, 1, 8, };
	static int a03[] = { 2, 3, 4, 1, 0, 5, 8, 7, 6, };
	static int a04[] = { 2, 1, 8, 3, 0, 7, 4, 5, 6, };
	static int a05[] = { 8, 7, 6, 1, 0, 5, 2, 3, 4, };
	static int a06[] = { 6, 5, 4, 7, 0, 3, 8, 1, 2, };
	static int a07[] = { 4, 3, 2, 5, 0, 1, 6, 7, 8, };
	static int a08[] = { 0, 1, 2, 5, 4, 3, 6, 7, 8, };
	static int a09[] = { 0, 1, 2, 7, 6, 3, 8, 5, 4, };
	static int a10[] = { 0, 1, 2, 7, 8, 3, 6, 5, 4, };
	static int a11[] = { 0, 5, 6, 1, 4, 7, 2, 3, 8, };
	static int a12[] = { 0, 7, 8, 1, 6, 5, 2, 3, 4, };
	static int a13[] = { 0, 7, 6, 1, 8, 5, 2, 3, 4, };
	static int a14[] = { 6, 7, 8, 5, 4, 3, 0, 1, 2, };
	static int a15[] = { 8, 5, 4, 7, 6, 3, 0, 1, 2, };
	static int a16[] = { 6, 5, 4, 7, 8, 3, 0, 1, 2, };
	static int a17[] = { 2, 3, 8, 1, 4, 7, 0, 5, 6, };
	static int a18[] = { 2, 3, 4, 1, 6, 5, 0, 7, 8, };
	static int a19[] = { 2, 3, 4, 1, 8, 5, 0, 7, 6, };
	static int a20[] = { 2, 1, 0, 3, 4, 5, 8, 7, 6, };
	static int a21[] = { 2, 1, 0, 3, 6, 7, 4, 5, 8, };
	static int a22[] = { 2, 1, 0, 3, 8, 7, 4, 5, 6, };
	static int a23[] = { 6, 5, 0, 7, 4, 1, 8, 3, 2, };
	static int a24[] = { 8, 7, 0, 5, 6, 1, 4, 3, 2, };
	static int a25[] = { 6, 7, 0, 5, 8, 1, 4, 3, 2, };
	static int a26[] = { 8, 7, 6, 3, 4, 5, 2, 1, 0, };
	static int a27[] = { 4, 5, 8, 3, 6, 7, 2, 1, 0, };
	static int a28[] = { 4, 5, 6, 3, 8, 7, 2, 1, 0, };
	static int a29[] = { 8, 3, 2, 7, 4, 1, 6, 5, 0, };
	static int a30[] = { 4, 3, 2, 5, 6, 1, 8, 7, 0, };
	static int a31[] = { 4, 3, 2, 5, 8, 1, 6, 7, 0, };
	static int * layouts[] = {
		a00, a01, a02, a03, a04, a05, a06, a07,
		a08, a09, a10, a11, a12, a13, a14, a15,
		a16, a17, a18, a19, a20, a21, a22, a23,
		a24, a25, a26, a27, a28, a29, a30, a31,
	};
	int * a = layouts[rand() % 32];

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
		if(monster.type->faction == Monster::PLAYER) {
			std::swap(monster, monsters.front());
			log("Player found.");
			break;
		}
	}
}


}
