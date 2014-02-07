#include "../src/info.h"
#include "../src/items.h"
#include "../src/monsters.h"
#include "../src/test.h"

SUITE(info) {

struct ItemBase {
	Chthon::ItemType type;
	Chthon::Item base;
	ItemBase() : type(Chthon::ItemType::Builder("type").sprite(1).name("name")), base(&type) { }
};

TEST_FIXTURE(ItemBase, should_mark_item_as_passable)
{
	ASSERT(Chthon::Info(base).passable);
}

TEST_FIXTURE(ItemBase, should_mark_item_as_transparent)
{
	ASSERT(Chthon::Info(base).transparent);
}


struct MonsterBase {
	Chthon::MonsterType type;
	Chthon::Monster base;
	MonsterBase() : type(Chthon::MonsterType::Builder("type").sprite(1).name("name")), base(&type) { }
};

TEST_FIXTURE(MonsterBase, should_mark_monster_as_impassable)
{
	ASSERT(!Chthon::Info(base).passable);
}

TEST_FIXTURE(MonsterBase, should_mark_monster_as_transparent)
{
	ASSERT(Chthon::Info(base).transparent);
}

}
