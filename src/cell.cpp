#include "cell.h"

namespace Chthon {

CellType::CellType(const std::string & type_id)
	: id(type_id), sprite(0), passable(false), transparent(false), hurts(false)
{
}

CellType::Builder & CellType::Builder::sprite(const int & value) { result.sprite = value; return *this; }
CellType::Builder & CellType::Builder::passable(bool value) { result.passable = value; return *this; }
CellType::Builder & CellType::Builder::hurts(bool value) { result.hurts = value; return *this; }
CellType::Builder & CellType::Builder::transparent(bool value) { result.transparent = value; return *this; }
CellType::Builder & CellType::Builder::name(const std::string & value) { result.name = value; return *this; }


Cell::Cell(const CellType * cell_type)
	: type(cell_type), visible(false), seen_sprite(0)
{
}

}
