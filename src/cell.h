#pragma once
#include "types.h"
#include "util.h"
#include <string>

namespace Chthon {

struct CellType {
	std::string id;
	int sprite;
	std::string name;
	bool passable;
	bool transparent;
	bool hurts;
	CellType(const std::string & type_id = std::string());

	struct Builder;
};
struct CellType::Builder {
	CellType value_result;
	CellType & result;
	Builder(const std::string & type_id) : value_result(type_id), result(value_result) {}
	Builder(CellType & type) : result(type) {}
	operator CellType() { return result; }
	Builder & sprite(const int & sprite);
	Builder & name(const std::string & value);
	Builder & passable(bool value);
	Builder & transparent(bool value);
	Builder & hurts(bool value);
};

struct Cell {
	typedef CellType Type;
	TypePtr<Type> type;
	bool visible;
	int seen_sprite;
	explicit Cell(const CellType * cell_type = nullptr);
};

}
