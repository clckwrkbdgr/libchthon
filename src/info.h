#pragma once
#include "map.h"
#include "point.h"
#include "util.h"
#include <string>
#include <vector>

namespace Chthon { /// @defgroup Info Object info
/// @{

class Item;
class ItemType;
class Object;
class ObjectType;
class Monster;
class MonsterType;
class Cell;
class CellType;

/** Represents generic info for classes, like type id, name, sprite, and other basic properties.
 */
struct Info {
	std::string id;
	int sprite;
	std::string name;
	bool passable;
	bool transparent;

	/// Constructs invalid info.
	Info();
	/// Constructs info for Item. Item is passable and transparent.
	Info(const Item & base);
	Info(const ItemType & base);
	/// Constructs info for Object.
	Info(const Object & base);
	Info(const ObjectType & base);
	/// Constructs info for Monster. Monster if impassable and transparent.
	Info(const Monster & base);
	Info(const MonsterType & base);
	/// Constructs info for Cell's type (conviniency usage).
	Info(const Cell & base);
	Info(const CellType & base);
	/// Returns true if info was created from valid value, otherwise returns false.
	bool valid() const;
};

/** Represents list of Info object that tied together at the same position.
 * Also can produce one info object, compiled from all list value (i.e. the _top/total_ info).
 */
struct CompiledInfo {
	const Point & pos;
	std::vector<Info> all_info;
	/// Prepares info list and specified target position.
	CompiledInfo(const Point & target_pos) : pos(target_pos) {}
	/// Appends (to the top) all items from container which have same position as the target.
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
	/// Appends (to the top) cell from map at the specified target position.
	CompiledInfo & in(const Map<Cell> & map);
	/// Returns compiled info for list.
	Info compiled() const;
};

/// @}
}
