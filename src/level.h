#pragma once
#include "map.h"
#include "monsters.h"
#include "objects.h"
#include "items.h"
#include "cell.h"
#include <vector>
#include <list>

namespace Chthon { /// @defgroup Level Level and dungeon bulder
/// @{

class Monster;
class Item;
class Object;
class Action;
class Game;
class CompiledInfo;

struct Level {
	Map<Cell> map;
	std::vector<Monster> monsters;
	std::vector<Item> items;
	std::vector<Object> objects;

	Level();
	~Level();
	Level(unsigned map_width, unsigned map_height);

	const CellType & cell_type_at(const Point & pos) const;
	CompiledInfo get_info(int x, int y) const;
	CompiledInfo get_info(const Point & pos) const;
	const Monster & get_player() const;
	Monster & get_player();
	std::list<Point> find_path(const Point & player_pos, const Point & target);
	void invalidate_fov(Monster & monster);
	void erase_dead_monsters();
};

struct DungeonBuilder {
	static void fill_room(Map<Cell> & map, const std::pair<Point, Point> & room, const CellType * type);
	static std::vector<Point> random_positions(const std::pair<Point, Point> & room, unsigned count);
	static std::pair<Point, Point> connect_rooms(Level & level, const std::pair<Point, Point> & a, const std::pair<Point, Point> & b, const CellType * type);
	static std::vector<std::pair<Point, Point> > shuffle_rooms(const std::vector<std::pair<Point, Point> > & rooms);
	static void pop_player_front(std::vector<Monster> & monsters);
};

/// @}
}
