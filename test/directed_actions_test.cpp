#include "mocks.h"
#include "../src/actions.h"
#include "../src/game.h"
#include "../src/test.h"
using Chthon::Point;
using Chthon::GameEvent;
using Chthon::Action;

namespace Chthon {

static std::string to_string(const Chthon::TypePtr<Chthon::ItemType> & type)
{
	return type->id;
}

}

SUITE(move) {
using GameMocks::GameWithDummyAndObjects;
using Chthon::Move;

TEST_FIXTURE(GameWithDummyAndObjects, should_move_when_cell_is_empty)
{
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 0));
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_move_into_impassable_cell)
{
	game.add_cell_type("floor").name("wall").passable(false);
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::BUMPS_INTO);
		EQUAL(e.target.name, "wall");
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_move_into_monster)
{
	game.add_monster("stub").pos(Point(1, 0));
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::BUMPS_INTO);
		EQUAL(e.target.name, "stub");
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_move_into_impassable_object)
{
	game.add_object("stub").pos(Point(1, 0));
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::BUMPS_INTO);
		EQUAL(e.target.name, "stub");
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_move_into_opened_object)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(true);
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 0));
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_move_into_closed_object)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(false);
	Move(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().pos, Point(1, 1));
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::BUMPS_INTO);
		EQUAL(e.target.name, "door");
	} DONE(e);
}

}

SUITE(drink) {
using GameMocks::GameWithDummyAndObjects;
using Chthon::Drink;

TEST_FIXTURE(GameWithDummyAndObjects, should_not_drink_monsters)
{
	game.add_monster("stub").pos(Point(1, 0));
	CATCH(Drink(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::CANNOT_DRINK);
	}
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_drink_not_drinkable_objects)
{
	game.add_object("pot").pos(Point(1, 0));
	CATCH(Drink(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_DRINK);
	}
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_drink_at_empty_cell)
{
	CATCH(Drink(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_DRINK);
	}
}

TEST_FIXTURE(GameWithDummyAndObjects, should_drink_fountains)
{
	game.add_object("well").pos(Point(1, 0));
	Drink(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::DRINKS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_heal_from_fountains)
{
	game.add_object("well").pos(Point(1, 0));
	dummy().hp -= 5;
	Drink(Point(0, -1)).commit(dummy(), game);
	EQUAL(dummy().hp, 96);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::DRINKS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::HEALS);
	} DONE(e);
}

}

SUITE(open) {
using GameMocks::GameWithDummyAndObjects;
using Chthon::Open;

TEST_FIXTURE(GameWithDummyAndObjects, should_not_open_already_opened_doors)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(true);
	CATCH(Open(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::ALREADY_OPENED);
		EQUAL(e.actor.name, "door");
	}
	ASSERT(game.current_level().objects[0].opened());
}

TEST_FIXTURE(GameWithDummyAndObjects, should_open_closed_doors)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(false);
	Open(Point(0, -1)).commit(dummy(), game);
	ASSERT(game.current_level().objects[0].opened());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::OPENS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_open_locked_doors_without_a_key)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(false).locked(true).lock_type(1);
	CATCH(Open(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::LOCKED);
		EQUAL(e.actor.name, "door");
	}
	ASSERT(game.current_level().objects[0].locked);
	ASSERT(!game.current_level().objects[0].opened());
}

TEST_FIXTURE(GameWithDummyAndObjects, should_open_locked_doors_with_a_key)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(false).locked(true).lock_type(1);
	dummy().inventory.set_item(1, Chthon::Item::Builder(game.item_type("key")).key_type(1));
	Open(Point(0, -1)).commit(dummy(), game);
	ASSERT(!game.current_level().objects[0].locked);
	ASSERT(game.current_level().objects[0].opened());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::UNLOCKS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::OPENS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_open_empty_cell)
{
	CATCH(Open(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_OPEN);
	}
}

TEST_FIXTURE(GameWithDummyAndObjects, should_open_containers_and_drop_items)
{
	Chthon::Item item(game.item_type("key"));
	game.add_object("pot").pos(Point(1, 0)).item(item);
	Open(Point(0, -1)).commit(dummy(), game);
	EQUAL(game.current_level().items[0].type, item.type);
	ASSERT(game.current_level().objects[0].items.empty());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TAKES_FROM);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_open_empty_containers)
{
	game.add_object("pot").pos(Point(1, 0));
	CATCH(Open(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::HAS_NO_ITEMS);
		EQUAL(e.actor.name, "pot");
	}
	ASSERT(game.current_level().items.empty());
	ASSERT(game.current_level().objects[0].items.empty());
}

}

SUITE(close) {
using GameMocks::GameWithDummyAndObjects;
using Chthon::Close;

TEST_FIXTURE(GameWithDummyAndObjects, should_close_opened_doors)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(true);
	Close(Point(0, -1)).commit(dummy(), game);
	ASSERT(!game.current_level().objects[0].opened());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::CLOSES);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_close_already_closed_doors)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(false);
	CATCH(Close(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::ALREADY_CLOSED);
	}
	ASSERT(!game.current_level().objects[0].opened());
}

TEST_FIXTURE(GameWithDummyAndObjects, should_not_close_empty_cell)
{
	CATCH(Close(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_CLOSE);
	}
}

}

SUITE(swing) {
using GameMocks::GameWithDummyAndObjects;
using Chthon::Swing;

TEST_FIXTURE(GameWithDummyAndObjects, should_hit_impassable_cells_on_swing)
{
	game.add_cell_type("wall").name("wall").passable(false);
	game.current_level().map.cell(Point(1, 0)) = Chthon::Cell(game.cell_type("wall"));
	Swing(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_open_closed_doors_on_swing)
{
	game.add_object("closed_door", "opened_door").pos(Point(1, 0)).opened(false);
	Swing(Point(0, -1)).commit(dummy(), game);
	ASSERT(game.current_level().objects[0].opened());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS);
		EQUAL(e.target.name, "door");
	} NEXT(e) {
		EQUAL(e.type, GameEvent::OPENS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_hit_monsters_on_swing)
{
	game.add_monster("stub").pos(Point(1, 0));
	Swing(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 0);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndObjects, should_swing_at_nothing_at_empty_cell)
{
	Swing(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::SWINGS_AT_NOTHING);
	} DONE(e);
}

}

SUITE(fire) {
using GameMocks::GameWithDummyWieldingAndWearing;
using Chthon::Fire;

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_throw_if_wields_nothing)
{
	dummy().inventory.unwield();
	CATCH(Fire(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_THROW);
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_monster_when_thrown)
{
	Fire(Point(0, -1)).commit(dummy(), game);
	ASSERT(!dummy().inventory.get_item(0).valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_from_monster_when_thrown)
{
	Fire(Point(0, -1)).commit(dummy(), game);
	ASSERT(!dummy().inventory.wielded_item().valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_opaque_cell_and_drop_item_before_it)
{
	game.add_cell_type("wall").name("wall").transparent(false);
	game.current_level().map.cell(Point(1, 0)) = Chthon::Cell(game.cell_type("wall"));
	Fire(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::THROWS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::HITS);
		EQUAL(e.actor.name, "spear");
	} DONE(e);
	ASSERT(!game.current_level().items.empty());
	EQUAL(game.current_level().items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_impassable_object_and_drop_item_before_it)
{
	game.add_object("door").pos(Point(1, 0));
	Fire(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::THROWS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::HITS);
		EQUAL(e.actor.name, "spear");
	} DONE(e);
	ASSERT(!game.current_level().items.empty());
	EQUAL(game.current_level().items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_container_and_drop_item_in_it)
{
	game.add_object("pot").pos(Point(1, 0));
	Fire(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::THROWS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::FALLS_INTO);
		EQUAL(e.actor.name, "spear");
	} DONE(e);
	ASSERT(game.current_level().items.empty());
	ASSERT(!game.current_level().objects[0].items.empty());
	EQUAL(game.current_level().objects[0].items[0].type->name, "spear");
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_fountain_and_erase_item_forever)
{
	game.add_object("well").pos(Point(1, 0));
	Fire(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::THROWS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::FALLS_INTO);
		EQUAL(e.actor.name, "spear");
	} DONE(e);
	ASSERT(game.current_level().items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_hit_monster_and_drop_item_under_it)
{
	game.add_monster("stub").pos(Point(1, 0));
	Fire(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::THROWS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.actor.name, "spear");
		EQUAL(e.target.name, "stub");
		EQUAL(e.amount, 3);
	} DONE(e);
	ASSERT(!game.current_level().items.empty());
	EQUAL(game.current_level().items[0].pos, Point(1, 0));
}

}

SUITE(put) {
using GameMocks::GameWithDummyWieldingAndWearing;
using Chthon::Put;

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_put_if_wields_nothing)
{
	dummy().inventory.unwield();
	CATCH(Put(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_PUT);
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_monster_when_put)
{
	Put(Point(0, -1)).commit(dummy(), game);
	ASSERT(!dummy().inventory.get_item(0).valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_from_monster_when_put)
{
	Put(Point(0, -1)).commit(dummy(), game);
	ASSERT(!dummy().inventory.wielded_item().valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_put_item_on_the_floor_if_passable)
{
	Put(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} DONE(e);
	ASSERT(!game.current_level().items.empty());
	EQUAL(game.current_level().items[0].pos, Point(1, 1));
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_refill_item_if_emptyable_and_object_is_drinkable)
{
	game.add_object("well").pos(Point(1, 1));
	dummy().inventory.wield(3);
	Put(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::REFILLS);
	} DONE(e);
	ASSERT(dummy().inventory.has_item(3));
	ASSERT(dummy().inventory.get_item(3).is_full());
	ASSERT(game.current_level().items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_refill_already_full_item)
{
	game.add_object("well").pos(Point(1, 1));
	dummy().inventory.get_item(3).make_full();
	dummy().inventory.wield(3);
	CATCH(Put(Point(0, -1)).commit(dummy(), game), Action::Exception, e) {
		EQUAL(e.type, GameEvent::ALREADY_FULL);
	}
	ASSERT(dummy().inventory.has_item(3));
	ASSERT(dummy().inventory.get_item(3).is_full());
	ASSERT(game.current_level().items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_put_item_under_monster_if_target_is_impassable)
{
	game.add_cell_type("wall").name("wall").transparent(false);
	game.current_level().map.cell(Point(1, 1)) = Chthon::Cell(game.cell_type("wall"));
	Put(Point(0, -1)).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} DONE(e);
	ASSERT(!game.current_level().items.empty());
	EQUAL(game.current_level().items[0].pos, Point(1, 2));
}

}
