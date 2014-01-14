#pragma once
#include "map.h"
#include "util.h"
#include <string>
#include <vector>

namespace Chthon {

class Item;
class Object;
class Monster;
class Cell;
class CellType;
class Point;

struct Info {
	std::string id;
	int sprite;
	std::string name;
	bool passable;
	bool transparent;

	Info();
	Info(const Item & base);
	Info(const Object & base);
	Info(const Monster & base);
	Info(const Cell & base);
	Info(const CellType & base);
	bool valid() const;
};

struct CompiledInfo {
	const Point & pos;
	std::vector<Info> all_info;
	CompiledInfo(const Point & target_pos) : pos(target_pos) {}
	template<class T>
	CompiledInfo & in(const std::vector<T> & v)
	{
		foreach(const T & t, v) {
			if(t.pos == pos) {
				all_info.push_back(Info(t));
			}
		}
		return *this;
	}
	CompiledInfo & in(const Map<Cell> & map);
	Info compiled() const;
};

}
