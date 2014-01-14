#include "info.h"
#include "cell.h"
#include "items.h"
#include "monsters.h"
#include "objects.h"

namespace Chthon {

static std::string invalid_id = "";
static int invalid_sprite = 0;
static std::string invalid_name = "";
static bool invalid_impassable = false;
static bool invalid_passable = true;
static bool invalid_opaque = false;
static bool invalid_transparent = true;


Info::Info()
	: id(invalid_id), sprite(invalid_sprite), name(invalid_name), passable(invalid_impassable), transparent(invalid_opaque)
{
}

Info::Info(const Item & base)
	: id(base.type->id), sprite(base.type->sprite), name(base.type->name), passable(invalid_passable), transparent(invalid_transparent)
{
}

Info::Info(const Object & base)
	: id(base.type->id), sprite(base.type->sprite), name(base.type->name), passable(base.type->passable), transparent(base.type->transparent)
{
}

Info::Info(const Monster & base)
	: id(base.type->id), sprite(base.type->sprite), name(base.type->name), passable(invalid_impassable), transparent(invalid_transparent)
{
}

Info::Info(const Cell & base)
	: id(base.type->id), sprite(base.type->sprite), name(base.type->name), passable(base.type->passable), transparent(base.type->transparent)
{
}

Info::Info(const CellType & base)
	: id(base.id), sprite(base.sprite), name(base.name), passable(base.passable), transparent(base.transparent)
{
}

bool Info::valid() const
{
	return !id.empty();
}


CompiledInfo & CompiledInfo::in(const Map<Cell> & map)
{
	if(map.valid(pos)) {
		all_info.push_back(Info(map.cell(pos)));
	}
	return *this;
}

Info CompiledInfo::compiled() const
{
	Info result;
	result.passable = true;
	result.transparent = true;
	foreach(const Info & info, all_info) {
		if(result.id.empty()) {
			result.id = info.id;
		}
		if(result.sprite == 0) {
			result.sprite = info.sprite;
		}
		if(result.name.empty()) {
			result.name = info.name;
		}
		result.passable = result.passable && info.passable;
		result.transparent = result.transparent && info.transparent;
	}
	return result.valid() ? result : Info();
}


}
