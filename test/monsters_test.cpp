#include "../src/monsters.h"
#include "../src/test.h"

using namespace Chthon::UnitTest;

namespace Chthon {

SUITE(monsters) {

TEST(monster_with_nonzero_hp_should_be_alive)
{
	Monster monster;
	monster.hp = 1;
	ASSERT(!monster.is_dead());
}

TEST(monster_with_zero_hp_should_die)
{
	Monster monster;
	monster.hp = 0;
	ASSERT(monster.is_dead());
}

TEST(should_heal_monsters)
{
	MonsterType type;
	type.max_hp = 3;
	Monster monster(&type);
	monster.hp = 1;
	bool ok = monster.heal_by(1);
	ASSERT(ok);
	EQUAL(monster.hp, 2);
}

TEST(should_heal_monsters_up_to_max_hp)
{
	MonsterType type;
	type.max_hp = 3;
	Monster monster(&type);
	monster.hp = 1;
	bool ok = monster.heal_by(10);
	ASSERT(ok);
	EQUAL(monster.hp, 3);
}

TEST(should_not_heal_monsters_when_unharmed)
{
	MonsterType type;
	type.max_hp = 3;
	Monster monster(&type);
	bool ok = monster.heal_by(10);
	ASSERT(!ok);
	EQUAL(monster.hp, 3);
}

TEST(monster_without_equipment_should_have_base_damage)
{
	MonsterType type;
	type.hit_strength = 3;
	Monster monster(&type);
	EQUAL(monster.damage(), 3);
}

TEST(monster_with_equipment_should_have_weapon_damage_instead)
{
	MonsterType type;
	type.hit_strength = 3;
	Monster monster(&type);
	ItemType weapon = ItemType::Builder("weapon").damage(1).sprite(1);
	monster.inventory.insert(&weapon);
	monster.inventory.wield(0);
	EQUAL(monster.damage(), 1);
}

}

}
