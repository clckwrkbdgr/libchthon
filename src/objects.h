#pragma once
#include "util.h"
#include "items.h"

namespace Chthon {

struct ObjectType {
	std::string id;
	int sprite;
	std::string name;
	bool passable, transparent;
	bool containable;
	bool drinkable;
	bool transporting;
	bool triggerable;
	bool openable;
	ObjectType(const std::string & type_id = std::string());

	struct Builder;
};
struct ObjectType::Builder {
	ObjectType value_result;
	ObjectType & result;
	Builder(const std::string & type_id) : value_result(type_id), result(value_result) {}
	Builder(ObjectType & type) : result(type) {}
	operator ObjectType() { return result; }
	Builder & sprite(const int & value);
	Builder & name(const std::string & value);
	Builder & passable();
	Builder & transparent();
	Builder & containable();
	Builder & drinkable();
	Builder & transporting();
	Builder & triggerable();
	Builder & openable();
};

struct Object {
	typedef ObjectType Type;
	TypePtr<Type> type;
	TypePtr<Type> closed_type, opened_type;

	Point pos;
	std::vector<Item> items;
	int up_destination, down_destination;
	bool locked;
	int lock_type;
	Object(const Type * object_type = nullptr);
	Object(const Type * closed_object_type, const Type * opened_object_type);
	bool valid() const;
	bool open();
	bool close();
	bool opened() const;
	bool is_exit_up() const;
	bool is_exit_down() const;

	struct Builder;
};
struct Object::Builder {
	Object value_result;
	Object & result;
	Builder(const Type * type) : value_result(type), result(value_result) {}
	Builder(const Type * closed_type, const Type * opened_type) : value_result(closed_type, opened_type), result(value_result) {}
	Builder(Object & object) : result(object) {}
	operator Object() { return result; }
	Builder & pos(const Point & value);
	Builder & item(const Item & value);
	Builder & up_destination(int value);
	Builder & down_destination(int value);
	Builder & opened(bool value);
	Builder & locked(bool value);
	Builder & lock_type(int value);
};

}
