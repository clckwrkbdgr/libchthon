#include "mocks.h"
#include "../game.h"
#include "../monsters.h"
#include "../cell.h"
#include "../format.h"
#include "../test.h"

using namespace Chthon::UnitTest;

namespace Chthon {

using namespace GameMocks;

SUITE(game) {

TEST_FIXTURE(GameWithDummyOnTrap, should_trigger_trap_if_trap_is_set)
{
	game.process_environment(dummy());
	ASSERT(game.level().objects.front().items.empty());
}

TEST_FIXTURE(GameWithDummyOnTrap, should_hurt_monster_if_trap_is_set)
{
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TRIGGERS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyOnTrap, should_leave_bolt_if_trap_is_set)
{
	game.process_environment(dummy());
	EQUAL(game.level().items.front().type->sprite, 1);
}

TEST_FIXTURE(GameWithDummyOnTrap, should_not_hurt_monster_if_trap_is_triggered_already)
{
	game.level().objects.front().items.clear();
	game.process_environment(dummy());
	EQUAL(dummy().hp, 100);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::TRAP_IS_OUT_OF_ITEMS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_hurt_monster_if_cell_hurts)
{
	game.add_cell_type("floor").hurts(true);
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HURTS);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_hurt_monster_is_poisoned)
{
	dummy().poisoning = 10;
	game.process_environment(dummy());
	EQUAL(dummy().hp, 99);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::IS_HURT_BY_POISONING);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_decrease_poisoning_each_turn)
{
	dummy().poisoning = 10;
	game.process_environment(dummy());
	EQUAL(dummy().poisoning, 9);
}


TEST_FIXTURE(GameWithDummy, should_drop_loot_if_monster_is_dead)
{
	game.die(dummy());
	EQUAL(game.level().items.front().pos, dummy().pos);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DIED);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_end_game_if_player_is_dead)
{
	game.die(player());
	EQUAL(game.state, Game::PLAYER_DIED);
}


TEST_FIXTURE(GameWithDummy, should_hurt_monster)
{
	game.hurt(dummy(), 5);
	EQUAL(dummy().hp, 95);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
		EQUAL(e.amount, 5);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_hurt_if_damage_exceeds_defence)
{
	dummy().inventory.wear(0);
	game.hurt(dummy(), 5);
	EQUAL(dummy().hp, 98);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
		EQUAL(e.amount, 2);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_hurt_at_full_damage_if_piercing)
{
	dummy().inventory.wear(0);
	game.hurt(dummy(), 5, true);
	EQUAL(dummy().hp, 95);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
		EQUAL(e.amount, 5);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummy, should_die_if_hurts_too_much)
{
	game.hurt(dummy(), 105);
	ASSERT(dummy().is_dead());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::LOSES_HEALTH);
		EQUAL(e.amount, 105);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DIED);
	} DONE(e);
}


TEST_FIXTURE(GameWithDummyAndKiller, should_hit_monster)
{
	game.hit(killer(), dummy(), 5);
	EQUAL(dummy().hp, 95);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 5);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_hit_if_damage_exceeds_defence)
{
	dummy().inventory.wear(0);
	game.hit(killer(), dummy(), 5);
	EQUAL(dummy().hp, 98);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 2);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster)
{
	game.hit(poisoner(), dummy(), 5);
	EQUAL(dummy().poisoning, 5);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 5);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::POISONS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_poison_monster_no_more_than_some_max)
{
	dummy().poisoning =  3;
	game.hit(poisoner(), dummy(), 5);
	EQUAL(dummy().poisoning, 5);
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 5);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::POISONS);
	} DONE(e);
}

TEST_FIXTURE(GameWithDummyAndKiller, should_die_if_hit_was_too_much)
{
	game.hit(killer(), dummy(), 105);
	ASSERT(dummy().is_dead());
	TEST_CONTAINER(game.events, e) {
		EQUAL(e.type, GameEvent::HITS_FOR_HEALTH);
		EQUAL(e.amount, 105);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DROPS_AT);
	} NEXT(e) {
		EQUAL(e.type, GameEvent::DIED);
	} DONE(e);
}

}

}
