#include "mocks.h"
#include "../src/level.h"
#include "../src/game.h"
#include "../src/test.h"
using Chthon::Point;

SUITE(dungeon) {
using GameMocks::GameWithLevels;

TEST_FIXTURE(GameWithLevels, should_save_current_level_as_visited)
{
	game.go_to_level(1);
	game.go_to_level(2);
	EQUAL(game.levels.count(2), (unsigned)1);
}

TEST_FIXTURE(GameWithLevels, should_restore_player_from_the_old_level_at_new_pos)
{
	game.go_to_level(1);
	game.go_to_level(2);
	EQUAL(game.current_level().get_player().pos, Point(2, 2));
}

TEST_FIXTURE(GameWithLevels, should_restore_previously_visited_level)
{
	game.go_to_level(1);
	game.current_level().get_player().hp = 3;
	game.go_to_level(2);
	game.go_to_level(1);
	EQUAL(game.current_level().get_player().hp, 3);
}

TEST_FIXTURE(GameWithLevels, should_generated_newly_visited_level)
{
	game.go_to_level(1);
	ASSERT(game.current_level().get_player().type);
	EQUAL(game.current_level().get_player().type->sprite, 1);
}

}

SUITE(level) {
using GameMocks::Game2x2;
using GameMocks::LevelWithPath;
using GameMocks::LevelForSeeing;

TEST_FIXTURE(Game2x2, impassable_cells_should_be_impassable)
{
	game.add_cell_type("floor").passable(false);
	ASSERT(!game.current_level().get_info(0, 0).compiled().passable);
}

TEST_FIXTURE(Game2x2, monsters_should_be_impassable)
{
	game.add_monster("monster").pos(Point(1, 1));
	ASSERT(!game.current_level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, items_should_be_passable)
{
	game.add_item("item").pos(Point(1, 1));
	ASSERT(game.current_level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, impassable_objects_should_be_impassable)
{
	game.add_object("stone").pos(Point(1, 1)).opened(false);
	ASSERT(!game.current_level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, passable_objects_should_be_passable)
{
	game.add_object("passable").pos(Point(1, 1));
	ASSERT(game.current_level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, passable_cells_should_be_passable)
{
	ASSERT(game.current_level().get_info(1, 1).compiled().passable);
}


TEST_FIXTURE(Game2x2, opaque_cells_should_be_opaque)
{
	game.add_cell_type("floor").transparent(false);
	ASSERT(!game.current_level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, opaque_objects_should_be_opaque)
{
	game.add_object("stone").pos(Point(1, 1)).opened(false);
	ASSERT(!game.current_level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, transparent_objects_should_be_transparent)
{
	game.add_object("transparent").pos(Point(1, 1));
	ASSERT(game.current_level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, monsters_should_be_transparent)
{
	game.add_monster("monster").pos(Point(1, 1));
	ASSERT(game.current_level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, items_should_be_transparent)
{
	game.add_item("item").pos(Point(1, 1));
	ASSERT(game.current_level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, transparent_cells_should_be_transparent)
{
	ASSERT(game.current_level().get_info(1, 1).compiled().transparent);
}


TEST_FIXTURE(Game2x2, monster_should_be_on_top_of_all)
{
	game.add_object("passable").pos(Point(1, 1));
	game.add_monster("monster").pos(Point(1, 1));
	game.add_item("item").pos(Point(1, 1));
	int sprite = game.current_level().get_info(1, 1).compiled().sprite;
	EQUAL(sprite, 3);
}

TEST_FIXTURE(Game2x2, items_should_be_on_top_of_objects)
{
	game.add_object("passable").pos(Point(1, 1));
	game.add_item("item").pos(Point(1, 1));
	int sprite = game.current_level().get_info(Point(1, 1)).compiled().sprite;
	EQUAL(sprite, 4);
}

TEST_FIXTURE(Game2x2, objects_should_be_below_everything)
{
	game.add_object("passable").pos(Point(1, 1));
	int sprite = game.current_level().get_info(Point(1, 1)).compiled().sprite;
	EQUAL(sprite, 2);
}


TEST_FIXTURE(Game2x2, should_get_player_from_monsters)
{
	game.add_monster("monster");
	Chthon::Monster & monster = game.current_level().get_player();
	ASSERT(monster.type);
	EQUAL(monster.type->faction, Chthon::Monster::PLAYER);
}

TEST_FIXTURE(Game2x2, should_get_player_from_const_monsters)
{
	game.add_monster("monster");
	const Chthon::Monster & monster = static_cast<const Chthon::Game &>(game).current_level().get_player();
	ASSERT(monster.type);
	EQUAL(monster.type->faction, Chthon::Monster::PLAYER);
}


TEST_FIXTURE(LevelWithPath, should_find_path_between_points)
{
	std::list<Point> path = game.current_level().find_path(Point(0, 3), Point(2, 3));
	TEST_CONTAINER(path, pos) {
		EQUAL(pos, Point(0, -1));
	} NEXT(pos) {
		EQUAL(pos, Point(1, -1));
	} NEXT(pos) {
		EQUAL(pos, Point(1,  0));
	} NEXT(pos) {
		EQUAL(pos, Point(1,  1));
	} NEXT(pos) {
		EQUAL(pos, Point(-1, 1));
	} DONE(pos);
}

TEST_FIXTURE(LevelWithPath, should_find_path_between_close_points)
{
	std::list<Point> path = game.current_level().find_path(Point(0, 3), Point(0, 2));
	TEST_CONTAINER(path, pos) {
		EQUAL(pos, Point(0, -1));
	} DONE(pos);
}

TEST_FIXTURE(LevelWithPath, should_not_find_path_if_target_is_the_same_as_start)
{
	std::list<Point> path = game.current_level().find_path(Point(0, 3), Point(0, 3));
	ASSERT(path.empty());
}

TEST_FIXTURE(LevelForSeeing, should_store_seen_sprites)
{
	game.current_level().invalidate_fov(game.current_level().get_player());
	EQUAL(game.current_level().map.cell(1, 0).seen_sprite, 1);
	EQUAL(game.current_level().map.cell(2, 0).seen_sprite, 2);
	EQUAL(game.current_level().map.cell(1, 1).seen_sprite, 2);
	EQUAL(game.current_level().map.cell(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_remember_sprite_instead_of_content)
{
	game.current_level().invalidate_fov(game.current_level().get_player());

	game.current_level().get_player().pos = Point(0, 1);
	game.current_level().invalidate_fov(game.current_level().get_player());
	EQUAL(game.current_level().map.cell(2, 1).seen_sprite, 100);
}


TEST(should_erase_dead_monsters)
{
	GameMocks::DummyDungeon game;
	game.current_level() = Chthon::Level(2, 2);
	game.add_monster_type("dead").max_hp(100).sprite(1);
	game.add_monster_type("alive").max_hp(100).sprite(2);
	game.add_monster("dead").pos(Point(1, 1)).hp(0);
	game.add_monster("alive").pos(Point(1, 1));
	game.current_level().erase_dead_monsters();
	EQUAL(game.current_level().monsters.size(), (unsigned)1);
	ASSERT(game.current_level().monsters[0].type);
	EQUAL(game.current_level().monsters[0].type->sprite, 2);
}

}
