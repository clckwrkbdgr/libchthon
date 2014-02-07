#include "../src/monsters.h"
#include "../src/test.h"

SUITE(monsters) {

TEST(monster_with_nonzero_hp_should_be_alive)
{
	Chthon::Monster monster;
	monster.hp = 1;
	ASSERT(!monster.is_dead());
}

TEST(monster_with_zero_hp_should_die)
{
	Chthon::Monster monster;
	monster.hp = 0;
	ASSERT(monster.is_dead());
}

TEST(should_heal_monsters)
{
	Chthon::MonsterType type;
	type.max_hp = 3;
	Chthon::Monster monster(&type);
	monster.hp = 1;
	bool ok = monster.heal_by(1);
	ASSERT(ok);
	EQUAL(monster.hp, 2);
}

TEST(should_heal_monsters_up_to_max_hp)
{
	Chthon::MonsterType type;
	type.max_hp = 3;
	Chthon::Monster monster(&type);
	monster.hp = 1;
	bool ok = monster.heal_by(10);
	ASSERT(ok);
	EQUAL(monster.hp, 3);
}

TEST(should_not_heal_monsters_when_unharmed)
{
	Chthon::MonsterType type;
	type.max_hp = 3;
	Chthon::Monster monster(&type);
	bool ok = monster.heal_by(10);
	ASSERT(!ok);
	EQUAL(monster.hp, 3);
}

TEST(monster_without_equipment_should_have_base_damage)
{
	Chthon::MonsterType type;
	type.hit_strength = 3;
	Chthon::Monster monster(&type);
	EQUAL(monster.damage(), 3);
}

TEST(monster_with_equipment_should_have_weapon_damage_instead)
{
	Chthon::MonsterType type;
	type.hit_strength = 3;
	Chthon::Monster monster(&type);
	Chthon::ItemType weapon = Chthon::ItemType::Builder("weapon").damage(1).sprite(1);
	monster.inventory.insert(&weapon);
	monster.inventory.wield(0);
	EQUAL(monster.damage(), 1);
}

}
