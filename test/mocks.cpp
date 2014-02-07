#include "mocks.h"
#include "../src/log.h"
using Chthon::Level;
using Chthon::Point;
using Chthon::Monster;
using Chthon::Map;
using Chthon::Cell;
using Chthon::Item;

namespace GameMocks {

DummyDungeon::DummyDungeon() : Game() {}
DummyDungeon::~DummyDungeon() {}
void DummyDungeon::generate(Level & /*level*/, int /*level_index*/) {}
void DummyDungeon::create_types(Game & /*game*/) {}

TestDungeon::TestDungeon(const Point & player_pos1, const Point & player_pos2)
	: generated(false), pos1(player_pos1), pos2(player_pos2),
	player_one(Chthon::MonsterType::Builder("player_one").sprite(1).faction(Monster::PLAYER)),
	player_two(Chthon::MonsterType::Builder("player_two").sprite(2).faction(Monster::PLAYER))
{
}
void TestDungeon::create_types(Game &) { }
void TestDungeon::generate(Level & level, int level_index)
{
	generated = true;
	level = Level(4, 4);
	if(level_index == 1) {
		level.monsters.push_back(Monster::Builder(&player_one).pos(pos1));
	} else {
		level.monsters.push_back(Monster::Builder(&player_two).pos(pos2));
	}
}


GameWithDummyWieldingAndWearing::GameWithDummyWieldingAndWearing()
	: game()
{
	game.current_level().map = Map<Cell>(2, 3);
	game.add_cell_type("floor").passable(true).transparent(true).name("floor");
	game.add_monster_type("dummy").max_hp(100).name("dummy");
	game.add_monster_type("stub").name("stub").max_hp(100);
	game.add_object_type("door").name("door");
	game.add_object_type("pot").name("pot").containable();
	game.add_object_type("well").name("well").drinkable();
	game.add_item_type("armor").sprite(1).wearable().defence(3).name("armor");
	game.add_item_type("spear").sprite(2).damage(3).name("spear");
	game.add_item_type("item").sprite(1).name("item");
	game.add_item_type("quest_item").sprite(1).name("item").quest();
	game.add_item_type("stub").sprite(2).name("stub");
	game.add_item_type("jacket").sprite(1).name("jacket").wearable();
	game.add_item_type("full_flask").sprite(1).name("water flask");
	game.add_item_type("empty_flask").sprite(2).name("empty flask");

	game.current_level().map.fill(Cell(game.cell_type("floor")));
	game.add_monster("dummy").pos(Point(1, 2)).item(Item(game.item_type("spear"))).item(Item(game.item_type("armor"))).wield(0).wear(1).item(game.item_type("jacket")).item(Item::Builder(game.item_type("full_flask"), game.item_type("empty_flask")).make_empty());
}
Monster & GameWithDummyWieldingAndWearing::dummy() { return game.current_level().monsters[0]; }

GameWithDummyWithItems::GameWithDummyWithItems()
	: game()
{
	game.current_level().map = Map<Cell>(2, 3);
	game.add_cell_type("floor").passable(true).transparent(true).name("floor");
	game.add_monster_type("dummy").max_hp(100).name("dummy");
	game.add_item_type("armor").sprite(1).wearable().defence(3).name("armor");
	game.add_item_type("spear").sprite(2).damage(3).name("spear");
	game.add_item_type("pot").sprite(1).name("pot");

	game.current_level().map.fill(Cell(game.cell_type("floor")));
	game.add_monster("dummy").pos(Point(1, 2)).item(game.item_type("spear")).item(game.item_type("armor")).item(game.item_type("pot")).item(game.item_type("pot"));
	dummy().inventory.take_item(2);
}
Monster & GameWithDummyWithItems::dummy() { return game.current_level().monsters[0]; }

GameWithDummyAndFood::GameWithDummyAndFood()
	: game()
{
	game.current_level().map = Map<Cell>(2, 2);
	game.add_monster_type("dummy").max_hp(100).name("dummy");

	game.add_item_type("armor").sprite(1).wearable().defence(3).name("armor").edible();
	game.add_item_type("spear").sprite(2).damage(3).name("spear").edible();
	game.add_item_type("junk").sprite(3).name("junk");
	game.add_item_type("food").sprite(4).name("food").edible();
	game.add_item_type("medkit").sprite(4).name("medkit").edible().healing(5);
	game.add_item_type("megasphere").sprite(4).name("megasphere").edible().healing(100);
	game.add_item_type("antidote").sprite(4).name("antidote").edible().antidote(5);
	game.add_item_type("full_flask").sprite(7).name("water flask").edible();
	game.add_item_type("empty_flask").sprite(8).name("empty flask");
	game.add_monster("dummy").hp(90).item(game.item_type("armor")).item(game.item_type("spear")).wear(0).wield(1).item(game.item_type("junk")).item(game.item_type("food")).item(game.item_type("medkit")).item(game.item_type("megasphere")).item(game.item_type("antidote")).item(Item()).item(Item(game.item_type("full_flask"), game.item_type("empty_flask"))).item(Item::Builder(game.item_type("full_flask"), game.item_type("empty_flask")).make_empty());
}
Monster & GameWithDummyAndFood::dummy() { return game.current_level().monsters[0]; }

GameWithDummyAndStairs::GameWithDummyAndStairs()
	: game(Point(1, 1), Point(2, 2))
{
	game.add_monster_type("dummy").name("dummy");
	game.add_object_type("stairs").name("stairs").transporting();
	game.add_item_type("yendor").name("Yendor").quest().sprite(1);
	game.current_level().map = Map<Cell>(2, 2);
	game.add_monster("dummy").pos(Point(1, 1));
	game.add_object("stairs").pos(Point(1, 1));
}
Monster & GameWithDummyAndStairs::dummy() { return game.current_level().monsters[0]; }
Chthon::Object & GameWithDummyAndStairs::stairs() { return game.current_level().objects[0]; }

GameWithDummyAndObjects::GameWithDummyAndObjects()
	: game()
{
	game.current_level().map = Map<Cell>(2, 2);
	game.add_cell_type("floor").name("floor").passable(true);
	game.add_monster_type("dummy").max_hp(100).name("dummy");
	game.add_monster_type("stub").name("stub");
	game.add_object_type("stub").name("stub");
	game.add_object_type("opened_door").name("door").transparent().passable().openable().sprite(1);
	game.add_object_type("closed_door").name("door").openable().sprite(2);
	game.add_object_type("pot").name("pot").containable();
	game.add_object_type("well").name("well").drinkable();
	game.add_item_type("key").name("item").sprite(1);
	game.current_level().map.fill(Cell(game.cell_type("floor")));
	game.add_monster("dummy").pos(Point(1, 1));
}
Monster & GameWithDummyAndObjects::dummy() { return game.current_level().monsters[0]; }

GameWithDummyOnTrap::GameWithDummyOnTrap()
	: game()
{
	game.add_monster_type("dummy").max_hp(100).name("dummy");
	game.current_level().map = Map<Cell>(2, 2);
	game.add_monster("dummy").pos(Point(1, 1));
	game.add_object_type("trap").name("trap").triggerable();
	game.add_item_type("item").name("item").sprite(1);
	game.add_object("trap").pos(Point(1, 1)).item(game.item_type("item"));
}
Monster & GameWithDummyOnTrap::dummy() { return game.current_level().monsters.front(); }

GameWithDummy::GameWithDummy()
	: game()
{
	game.current_level().map = Map<Cell>(2, 2);
	game.add_cell_type("floor");
	game.add_monster_type("dummy").max_hp(100).name("dummy");
	game.add_monster_type("player").max_hp(100).name("dummy").faction(Monster::PLAYER);

	game.current_level().map.fill(Cell(game.cell_type("floor")));
	game.add_item_type("armor").sprite(1).wearable().defence(3).name("item");
	game.add_monster("dummy").pos(Point(1, 1)).item(game.item_type("armor"));
	game.add_monster("player").pos(Point(1, 1)).item(game.item_type("armor"));
}
Monster & GameWithDummy::dummy() { return game.current_level().monsters.front(); }
Monster & GameWithDummy::player() { return game.current_level().monsters.back(); }

GameWithDummyAndKiller::GameWithDummyAndKiller()
	: game()
{
	game.add_monster_type("dummy").max_hp(100).name("dummy");
	game.add_monster_type("killer").max_hp(100).name("killer");
	game.add_monster_type("poisoner").max_hp(100).name("poisoner").poisonous(true);
	game.current_level().map = Map<Cell>(2, 2);
	game.add_item_type("armor").sprite(1).wearable().defence(3).name("item");
	game.add_monster("dummy").pos(Point(1, 1)).item(game.item_type("armor"));
	game.add_monster("killer").pos(Point(0, 1));
	game.add_monster("poisoner").pos(Point(1, 0));
}
Monster & GameWithDummyAndKiller::dummy() { return game.current_level().monsters[0]; }
Monster & GameWithDummyAndKiller::killer() { return game.current_level().monsters[1]; }
Monster & GameWithDummyAndKiller::poisoner() { return game.current_level().monsters[2]; }

Game2x2::Game2x2()
	: game()
{
	game.current_level() = Level(2, 2);
	game.add_cell_type("floor").sprite(1).passable(true).transparent(true);
	game.add_monster_type("monster").sprite(3).faction(Monster::PLAYER);
	game.add_object_type("stone").name("stone");
	game.add_object_type("passable").passable().sprite(2);
	game.add_object_type("transparent").transparent();
	game.add_item_type("item").sprite(4);
	game.current_level().map.fill(Cell(game.cell_type("floor")));
}

LevelWithPath::LevelWithPath()
	: game()
{
	game.current_level() = Level(4, 4);
	game.add_cell_type("floor").passable(true);
	game.add_cell_type("wall").passable(false);
	const Chthon::CellType * f = game.cell_type("floor");
	const Chthon::CellType * w = game.cell_type("wall");
	const Cell a[] = {
		Cell(f), Cell(w), Cell(f), Cell(f),
		Cell(w), Cell(f), Cell(f), Cell(w),
		Cell(f), Cell(w), Cell(w), Cell(f),
		Cell(f), Cell(w), Cell(f), Cell(w),
	};
	game.current_level().map.fill(a);
}

LevelForSeeing::LevelForSeeing()
	: game()
{
	game.current_level() = Level(3, 2);
	game.add_cell_type("floor").sprite(1).passable(true).transparent(true);
	game.add_cell_type("wall").sprite(2).passable(false).transparent(false);
	const Chthon::CellType * f = game.cell_type("floor");
	const Chthon::CellType * w = game.cell_type("wall");
	const Cell a[] = {
		Cell(w), Cell(f), Cell(w),
		Cell(f), Cell(w), Cell(f),
	};
	game.current_level().map.fill(a);
	game.add_monster_type("player").faction(Monster::PLAYER).sight(3).sprite(100);
	game.add_monster("player").pos(Point(2, 1));
}

}
