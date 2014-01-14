#include "../src/info.h"
#include "../src/items.h"
#include "../src/monsters.h"
#include "../src/test.h"

using namespace Chthon::UnitTest;

namespace Chthon {

SUITE(info) {

struct ItemBase {
	ItemType type;
	Item base;
	ItemBase() : type(ItemType::Builder("type").sprite(1).name("name")), base(&type) { }
};

TEST_FIXTURE(ItemBase, should_mark_item_as_passable)
{
	ASSERT(Info(base).passable);
}

TEST_FIXTURE(ItemBase, should_mark_item_as_transparent)
{
	ASSERT(Info(base).transparent);
}


struct MonsterBase {
	MonsterType type;
	Monster base;
	MonsterBase() : type(MonsterType::Builder("type").sprite(1).name("name")), base(&type) { }
};

TEST_FIXTURE(MonsterBase, should_mark_monster_as_impassable)
{
	ASSERT(!Info(base).passable);
}

TEST_FIXTURE(MonsterBase, should_mark_monster_as_transparent)
{
	ASSERT(Info(base).transparent);
}

}

}
