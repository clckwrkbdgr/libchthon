#pragma once
#include "items.h"
#include <list>

namespace Chthon {

class Action;

struct MonsterType {
	std::string id;
	int faction;
	int sprite;
	int sight;
	int ai;
	int max_hp;
	int hit_strength;
	std::string name;
	bool poisonous;
	MonsterType(const std::string & type_id = std::string());

	struct Builder;
};
struct MonsterType::Builder {
	MonsterType value_result;
	MonsterType & result;
	Builder(const std::string & type_id) : value_result(type_id), result(value_result) {}
	Builder(MonsterType & type) : result(type) {}
	operator MonsterType() { return result; }
	Builder & faction(int value);
	Builder & sprite(const int & sprite);
	Builder & sight(int value);
	Builder & ai(int value);
	Builder & max_hp(int value);
	Builder & name(const std::string & value);
	Builder & hit_strength(int value);
	Builder & poisonous(bool value);
};


struct Monster {
	enum Faction { NEUTRAL, PLAYER, MONSTER };
	typedef MonsterType Type;

	TypePtr<Type> type;
	Point pos;
	int hp;
	Inventory inventory;
	int poisoning;
	std::list<Action*> plan;
	Monster(const Type * monster_type = nullptr);
	~Monster();
	bool valid() const;
	bool is_dead() const { return hp <= 0; }
	int damage() const;
	void add_path(const std::list<Point> & path);
	bool heal_by(int hp_amount);

	struct Builder;
};
struct Monster::Builder {
	Monster value_result;
	Monster & result;
	Builder(const Type * type) : value_result(type), result(value_result) {}
	Builder(Monster & monster) : result(monster) {}
	operator Monster() { return result; }
	Builder & pos(const Point & value);
	Builder & hp(int value);
	Builder & item(const Item & value);
	Builder & wield(unsigned value);
	Builder & wear(unsigned value);
};

}
