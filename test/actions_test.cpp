#include "mocks.h"
#include "../src/actions.h"
#include "../src/game.h"
#include "../src/monsters.h"
#include "../src/cell.h"
#include "../src/format.h"
#include "../src/log.h"
#include "../src/test.h"
using Chthon::to_string;
using Chthon::Action;
using Chthon::GameEvent;

SUITE(drop) {
using GameMocks::GameWithDummyWieldingAndWearing;
using Chthon::Drop;

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_drop_if_nothing_to_drop)
{
	dummy().inventory.clear();
	CATCH(Drop(0).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_DROP);
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_drop_items_only_in_range)
{
	CATCH(Drop(4).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NO_SUCH_ITEM);
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_before_dropping)
{
	Drop(0).commit(dummy(), game);
	ASSERT(!dummy().inventory.wielded_item().valid());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::UNWIELDS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_before_dropping)
{
	Drop(1).commit(dummy(), game);
	ASSERT(!dummy().inventory.worn_item().valid());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TAKES_OFF);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_item_from_inventory_when_dropped)
{
	Drop(0).commit(dummy(), game);
	ASSERT(!dummy().inventory.get_item(0).valid());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_place_item_on_the_floor_when_dropped)
{
	Drop(0).commit(dummy(), game);
	EQUAL(game.current_level().items.size(), 1);
	EQUAL(game.current_level().items[0].type->name, "spear");
}

}

SUITE(grab) {
using Chthon::Grab;
using GameMocks::GameWithDummyWieldingAndWearing;

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_grab_if_floor_is_empty)
{
	CATCH(Grab().commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_GRAB);
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_put_grabbed_item_to_the_first_empty_slot)
{
	game.add_item("item").pos(Chthon::Point(1, 2));
	Grab().commit(dummy(), game);
	EQUAL(dummy().inventory.get_item(4).type->name, "item");
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::PICKS_UP_FROM);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_remove_grabbed_item_from_map)
{
	game.add_item("item").pos(Chthon::Point(1, 2));
	Grab().commit(dummy(), game);
	ASSERT(game.current_level().items.empty());
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_notify_if_quest_item)
{
	game.add_item("quest_item").pos(Chthon::Point(1, 2));
	Grab().commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::PICKS_UP_FROM);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::PICKED_UP_A_QUEST_ITEM);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_not_grab_item_if_inventory_is_full)
{
	for(int i = 2; i < 26; ++i) {
		dummy().inventory.insert(Chthon::Item(game.item_type("stub")));
	}
	game.add_item("item").pos(Chthon::Point(1, 2));
	CATCH(Grab().commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NO_SPACE_LEFT);
	}
}

}

SUITE(wield) {
using Chthon::Wield;
using GameMocks::GameWithDummyWithItems;
using GameMocks::GameWithDummyWieldingAndWearing;

TEST_FIXTURE(GameWithDummyWithItems, should_wield_any_item)
{
	Wield(0).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::WIELDS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wield_invalid_slot)
{
	CATCH(Wield(10).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NO_SUCH_ITEM);
	}
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wield_empty_slot)
{
	CATCH(Wield(2).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NO_SUCH_ITEM);
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_previous_item_before_wielding_new)
{
	Chthon::ItemType sword = Chthon::ItemType::Builder("sword").sprite(1).name("sword");
	dummy().inventory.set_item(2, Chthon::Item(&sword));
	Wield(2).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::UNWIELDS);
		EQUAL(e.target.name, "spear");
	} NEXT(e) {
		EQUAL(e.type, GameEvent::WIELDS);
		EQUAL(e.target.name, "sword");
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_before_wielding_it)
{
	Wield(1).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::UNWIELDS);
		EQUAL(e.target.name, "spear");
	} NEXT(e) {
		EQUAL(e.type, GameEvent::TAKES_OFF);
		EQUAL(e.target.name, "armor");
	} NEXT(e) {
		EQUAL(e.type, GameEvent::WIELDS);
		EQUAL(e.target.name, "armor");
	} DONE(e);
}

}

SUITE(unwield) {
using Chthon::Unwield;
using GameMocks::GameWithDummyWithItems;
using GameMocks::GameWithDummyWieldingAndWearing;

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_if_wielded)
{
	Unwield().commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::UNWIELDS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_unwield_item_if_not_wielded)
{
	CATCH(Unwield().commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_UNWIELD);
	}
}

}

SUITE(wear) {
using Chthon::Wear;
using GameMocks::GameWithDummyWithItems;
using GameMocks::GameWithDummyWieldingAndWearing;

TEST_FIXTURE(GameWithDummyWithItems, should_wear_any_item)
{
	Wear(1).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::WEARS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_invalid_slot)
{
	CATCH(Wear(10).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NO_SUCH_ITEM);
	}
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_empty_slot)
{
	CATCH(Wear(2).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NO_SUCH_ITEM);
	}
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_wear_unwearable_item)
{
	CATCH(Wear(3).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::CANNOT_WEAR);
		EQUAL(e.target.name, "pot");
	}
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_previous_item_before_wearing_new)
{
	Wear(2).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TAKES_OFF);
		EQUAL(e.target.name, "armor");
	} NEXT(e) {
		EQUAL(e.type, GameEvent::WEARS);
		EQUAL(e.target.name, "jacket");
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_unwield_item_before_wearing_it)
{
	dummy().inventory.wield(2);
	Wear(2).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::UNWIELDS);
		EQUAL(e.target.name, "jacket");
	} NEXT(e) {
		EQUAL(e.type, GameEvent::TAKES_OFF);
		EQUAL(e.target.name, "armor");
	} NEXT(e) {
		EQUAL(e.type, GameEvent::WEARS);
		EQUAL(e.target.name, "jacket");
	} DONE(e);
}

}

SUITE(take_off) {
using Chthon::TakeOff;
using GameMocks::GameWithDummyWithItems;
using GameMocks::GameWithDummyWieldingAndWearing;

TEST_FIXTURE(GameWithDummyWieldingAndWearing, should_take_off_item_if_worn)
{
	TakeOff().commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TAKES_OFF);
		EQUAL(e.target.name, "armor");
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyWithItems, should_not_take_off_item_if_not_worn)
{
	CATCH(TakeOff().commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NOTHING_TO_TAKE_OFF);
	}
}

}

SUITE(eat) {
using GameMocks::GameWithDummyAndFood;
using Chthon::Eat;

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_invalid_slot)
{
	CATCH(Eat(NONE).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NO_SUCH_ITEM);
	}
}

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_empty_slot)
{
	CATCH(Eat(EMPTY).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::NO_SUCH_ITEM);
	}
}

TEST_FIXTURE(GameWithDummyAndFood, should_not_eat_not_edible_item)
{
	CATCH(Eat(JUNK).commit(dummy(), game), const Action::Exception & e) {
		EQUAL(e.type, GameEvent::CANNOT_EAT);
		EQUAL(e.target.name, "junk");
	}
}

TEST_FIXTURE(GameWithDummyAndFood, should_take_off_item_before_eating)
{
	Eat(ARMOR).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TAKES_OFF);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::EATS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndFood, should_unwield_item_before_eating)
{
	Eat(SPEAR).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::UNWIELDS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::EATS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndFood, should_not_unwield_emptyable_item_when_eating)
{
	dummy().inventory.wield(FULL_FLASK);
	Eat(FULL_FLASK).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::EATS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::EMPTIES);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndFood, should_eat_items)
{
	Eat(FOOD).commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::EATS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndFood, should_heal_when_eating_healing_item)
{
	Eat(MEDKIT).commit(dummy(), game);
	EQUAL(dummy().hp, 95);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::EATS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::HEALS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndFood, should_heal_up_to_max_hp_when_eating_healing_item)
{
	Eat(MEGASPHERE).commit(dummy(), game);
	EQUAL(dummy().hp, 100);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::EATS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::HEALS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndFood, should_cure_poisoning_when_eating_antidote)
{
	dummy().poisoning = 10;
	Eat(ANTIDOTE).commit(dummy(), game);
	EQUAL(dummy().poisoning, 5);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::EATS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::CURES_POISONING);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndFood, should_cure_poisoning_to_the_end_when_eating_antidote)
{
	dummy().poisoning = 5;
	Eat(ANTIDOTE).commit(dummy(), game);
	EQUAL(dummy().poisoning, 0);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::EATS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::CURES_POISONING);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndFood, should_make_flask_empty_after_eating_it_and_not_remove_it)
{
	Eat(FULL_FLASK).commit(dummy(), game);
	ASSERT(dummy().inventory.has_item(FULL_FLASK));
	ASSERT(dummy().inventory.get_item(FULL_FLASK).is_empty());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::EATS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::EMPTIES);
	} DONE(e);
}

}

SUITE(go_up) {
using GameMocks::GameWithDummyAndStairs;
using Chthon::GoUp;

TEST_FIXTURE(GameWithDummyAndStairs, should_go_up_on_upstairs)
{
	stairs().up_destination = 1;
	GoUp().commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::GOES_UP);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_end_turn_when_going_up)
{
	stairs().up_destination = 1;
	GoUp().commit(dummy(), game);
	EQUAL(game.state, Chthon::Game::TURN_ENDED);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_send_to_quest_on_upstairs_to_the_surface)
{
	stairs().up_destination = -1;
	GoUp().commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::SHOULD_GET_QUEST_ITEM);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_win_game_on_upstairs_when_have_quest_item)
{
	dummy().inventory.insert(Chthon::Item(game.item_type("yendor")));
	stairs().up_destination = -1;
	GoUp().commit(dummy(), game);
	EQUAL(game.state, Chthon::Game::COMPLETED);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::WINS_GAME_WITH);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_generate_corresponding_level_when_going_up)
{
	stairs().up_destination = 1;
	GoUp().commit(dummy(), game);
	ASSERT(game.was_generated());
}

}

SUITE(go_down) {
using GameMocks::GameWithDummyAndStairs;
using Chthon::GoDown;

TEST_FIXTURE(GameWithDummyAndStairs, should_go_down_on_downstairs)
{
	stairs().down_destination = 1;
	GoDown().commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::GOES_DOWN);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_end_turn_when_going_down)
{
	stairs().down_destination = 1;
	GoDown().commit(dummy(), game);
	EQUAL(game.state, Chthon::Game::TURN_ENDED);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_send_to_quest_on_downstairs_to_the_surface)
{
	stairs().down_destination = -1;
	GoDown().commit(dummy(), game);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::SHOULD_GET_QUEST_ITEM);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_win_game_on_downstairs_when_have_quest_item)
{
	dummy().inventory.insert(Chthon::Item(game.item_type("yendor")));
	stairs().down_destination = -1;
	GoDown().commit(dummy(), game);
	EQUAL(game.state, Chthon::Game::COMPLETED);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::WINS_GAME_WITH);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndStairs, should_generate_corresponding_level_when_going_down)
{
	stairs().down_destination = 1;
	GoDown().commit(dummy(), game);
	ASSERT(game.was_generated());
}

}
