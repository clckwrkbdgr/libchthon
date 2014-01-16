#include "monsters.h"
#include "actions.h"
#include "util.h"

namespace Chthon {

MonsterType::MonsterType(const std::string & type_id)
	: id(type_id), faction(Monster::NEUTRAL), sprite(0), sight(0), ai(0), max_hp(1),
	hit_strength(0), poisonous(false)
{
}

MonsterType::Builder & MonsterType::Builder::faction(int value) { result.faction = value; return *this; }
MonsterType::Builder & MonsterType::Builder::sprite(const int & value) { result.sprite = value; return *this; }
MonsterType::Builder & MonsterType::Builder::sight(int value) { result.sight = value; return *this; }
MonsterType::Builder & MonsterType::Builder::ai(int value) { result.ai = value; return *this; }
MonsterType::Builder & MonsterType::Builder::max_hp(int value) { result.max_hp = value; return *this; }
MonsterType::Builder & MonsterType::Builder::name(const std::string & value) { result.name = value; return *this; }
MonsterType::Builder & MonsterType::Builder::hit_strength(int value) { result.hit_strength = value; return *this; }
MonsterType::Builder & MonsterType::Builder::poisonous(bool value) { result.poisonous = value; return *this; }


Monster::Monster(const Type * monster_type)
	: type(monster_type), hp(type->max_hp), poisoning(0)
{
}

bool Monster::valid() const
{
	return type.valid();
}

Monster::~Monster()
{
	foreach(Action * action, plan) {
		delete action;
	}
}

int Monster::damage() const
{
	if(inventory.wielded_item().valid()) {
		return inventory.wielded_item().type->damage;
	}
	return type->hit_strength;
}

void Monster::add_path(const std::list<Point> & path)
{
	foreach(const Point & shift, path) {
		plan.push_back(new Move(shift));
	}
}

bool Monster::heal_by(int hp_amount)
{
	if(hp < type->max_hp) {
		hp += hp_amount;
		hp = std::min(hp, type->max_hp);
		return true;
	}
	return false;
}

Monster::Builder & Monster::Builder::pos(const Point & value) { result.pos = value; return *this; }
Monster::Builder & Monster::Builder::hp(int value) { result.hp = (result.type->max_hp >= value) ? value : result.type->max_hp; return *this; }
Monster::Builder & Monster::Builder::item(const Item & value) { result.inventory.insert(value); return *this; }
Monster::Builder & Monster::Builder::wield(unsigned value) { result.inventory.wield(value); return *this; }
Monster::Builder & Monster::Builder::wear(unsigned value) { result.inventory.wear(value); return *this; }

}
