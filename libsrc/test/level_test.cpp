#include "mocks.h"
#include "../level.h"
#include "../game.h"
#include "../test.h"

using namespace Chthon::UnitTest;

namespace Chthon {

using namespace GameMocks;

SUITE(dungeon) {

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
	EQUAL(game.level().get_player().pos, Point(2, 2));
}

TEST_FIXTURE(GameWithLevels, should_restore_previously_visited_level)
{
	game.go_to_level(1);
	game.level().get_player().hp = 3;
	game.go_to_level(2);
	game.go_to_level(1);
	EQUAL(game.level().get_player().hp, 3);
}

TEST_FIXTURE(GameWithLevels, should_generated_newly_visited_level)
{
	game.go_to_level(1);
	EQUAL(game.level().get_player().type->sprite, 1);
}

}

SUITE(level) {

TEST_FIXTURE(Game2x2, impassable_cells_should_be_impassable)
{
	game.add_cell_type("floor").passable(false);
	ASSERT(!game.level().get_info(0, 0).compiled().passable);
}

TEST_FIXTURE(Game2x2, monsters_should_be_impassable)
{
	game.add_monster("monster").pos(Point(1, 1));
	ASSERT(!game.level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, items_should_be_passable)
{
	game.add_item("item").pos(Point(1, 1));
	ASSERT(game.level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, impassable_objects_should_be_impassable)
{
	game.add_object("stone").pos(Point(1, 1)).opened(false);
	ASSERT(!game.level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, passable_objects_should_be_passable)
{
	game.add_object("passable").pos(Point(1, 1));
	ASSERT(game.level().get_info(1, 1).compiled().passable);
}

TEST_FIXTURE(Game2x2, passable_cells_should_be_passable)
{
	ASSERT(game.level().get_info(1, 1).compiled().passable);
}


TEST_FIXTURE(Game2x2, opaque_cells_should_be_opaque)
{
	game.add_cell_type("floor").transparent(false);
	ASSERT(!game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, opaque_objects_should_be_opaque)
{
	game.add_object("stone").pos(Point(1, 1)).opened(false);
	ASSERT(!game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, transparent_objects_should_be_transparent)
{
	game.add_object("transparent").pos(Point(1, 1));
	ASSERT(game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, monsters_should_be_transparent)
{
	game.add_monster("monster").pos(Point(1, 1));
	ASSERT(game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, items_should_be_transparent)
{
	game.add_item("item").pos(Point(1, 1));
	ASSERT(game.level().get_info(1, 1).compiled().transparent);
}

TEST_FIXTURE(Game2x2, transparent_cells_should_be_transparent)
{
	ASSERT(game.level().get_info(1, 1).compiled().transparent);
}


TEST_FIXTURE(Game2x2, monster_should_be_on_top_of_all)
{
	game.add_object("passable").pos(Point(1, 1));
	game.add_monster("monster").pos(Point(1, 1));
	game.add_item("item").pos(Point(1, 1));
	int sprite = game.level().get_info(1, 1).compiled().sprite;
	EQUAL(sprite, 3);
}

TEST_FIXTURE(Game2x2, items_should_be_on_top_of_objects)
{
	game.add_object("passable").pos(Point(1, 1));
	game.add_item("item").pos(Point(1, 1));
	int sprite = game.level().get_info(Point(1, 1)).compiled().sprite;
	EQUAL(sprite, 4);
}

TEST_FIXTURE(Game2x2, objects_should_be_below_everything)
{
	game.add_object("passable").pos(Point(1, 1));
	int sprite = game.level().get_info(Point(1, 1)).compiled().sprite;
	EQUAL(sprite, 2);
}


TEST_FIXTURE(Game2x2, should_get_player_from_monsters)
{
	game.add_monster("monster");
	Monster & monster = game.level().get_player();
	EQUAL(monster.type->faction, Monster::PLAYER);
}

TEST_FIXTURE(Game2x2, should_get_player_from_const_monsters)
{
	game.add_monster("monster");
	const Monster & monster = static_cast<const Game &>(game).level().get_player();
	EQUAL(monster.type->faction, Monster::PLAYER);
}


TEST_FIXTURE(LevelWithPath, should_find_path_between_points)
{
	std::list<Point> path = game.level().find_path(Point(0, 3), Point(2, 3));
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
	std::list<Point> path = game.level().find_path(Point(0, 3), Point(0, 2));
	TEST_CONTAINER(path, pos) {
		EQUAL(pos, Point(0, -1));
	} DONE(pos);
}

TEST_FIXTURE(LevelWithPath, should_not_find_path_if_target_is_the_same_as_start)
{
	std::list<Point> path = game.level().find_path(Point(0, 3), Point(0, 3));
	ASSERT(path.empty());
}

TEST_FIXTURE(LevelForSeeing, should_store_seen_sprites)
{
	game.level().invalidate_fov(game.level().get_player());
	EQUAL(game.level().map.cell(1, 0).seen_sprite, 1);
	EQUAL(game.level().map.cell(2, 0).seen_sprite, 2);
	EQUAL(game.level().map.cell(1, 1).seen_sprite, 2);
	EQUAL(game.level().map.cell(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_remember_sprite_instead_of_content)
{
	game.level().invalidate_fov(game.level().get_player());

	game.level().get_player().pos = Point(0, 1);
	game.level().invalidate_fov(game.level().get_player());
	EQUAL(game.level().map.cell(2, 1).seen_sprite, 100);
}

TEST_FIXTURE(LevelForSeeing, should_calculate_visible_area_within_sight_radius)
{
	game.level().invalidate_fov(game.level().get_player());
	EQUAL(game.level().map.cell(0, 0).visible, false);
	EQUAL(game.level().map.cell(0, 1).visible, false);
	EQUAL(game.level().map.cell(1, 0).visible, true);
	EQUAL(game.level().map.cell(2, 0).visible, true);
	EQUAL(game.level().map.cell(1, 1).visible, true);
	EQUAL(game.level().map.cell(2, 1).visible, true);
}

TEST_FIXTURE(LevelForSeeing, should_not_see_through_opaque_cells)
{
	game.level().invalidate_fov(game.level().get_player());
	EQUAL(game.level().map.cell(0, 1).visible, false);
}


TEST(should_erase_dead_monsters)
{
	DummyDungeon game;
	game.level() = Level(2, 2);
	game.add_monster_type("dead").max_hp(100).sprite(1);
	game.add_monster_type("alive").max_hp(100).sprite(2);
	game.add_monster("dead").pos(Point(1, 1)).hp(0);
	game.add_monster("alive").pos(Point(1, 1));
	game.level().erase_dead_monsters();
	EQUAL(game.level().monsters.size(), (unsigned)1);
	EQUAL(game.level().monsters[0].type->sprite, 2);
}

}

}
