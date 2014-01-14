#pragma once
#include "../src/level.h"
#include "../src/game.h"

namespace Chthon {

namespace GameMocks {

struct DummyDungeon : public Game {
	DummyDungeon();
	virtual ~DummyDungeon();
	virtual void generate(Level & /*level*/, int /*level_index*/);
	virtual void create_types(Game & /*game*/);
};

class TestDungeon : public Game {
public:
	TestDungeon(const Point & player_pos1, const Point & player_pos2);
	virtual void create_types(Game & game);
	virtual void generate(Level & level, int level_index);
	bool was_generated() const { return generated; }
private:
	bool generated;
	Point pos1, pos2;
	MonsterType player_one;
	MonsterType player_two;
};


struct GameWithDummyWieldingAndWearing {
	DummyDungeon game;
	GameWithDummyWieldingAndWearing();
	Monster & dummy();
};

struct GameWithDummyWithItems {
	DummyDungeon game;
	GameWithDummyWithItems();
	Monster & dummy();
};

struct GameWithDummyAndFood {
	DummyDungeon game;
	enum { ARMOR, SPEAR, JUNK, FOOD, MEDKIT, MEGASPHERE, ANTIDOTE, FULL_FLASK, EMPTY_FLASK, EMPTY, NONE };
	GameWithDummyAndFood();
	Monster & dummy();
};

struct GameWithDummyAndStairs {
	TestDungeon game;
	GameWithDummyAndStairs();
	Monster & dummy();
	Object & stairs();
};

struct GameWithDummyAndObjects {
	DummyDungeon game;
	GameWithDummyAndObjects();
	Monster & dummy();
};

struct GameWithDummyOnTrap {
	DummyDungeon game;
	GameWithDummyOnTrap();
	Monster & dummy();
};

struct GameWithDummy {
	DummyDungeon game;
	GameWithDummy();
	Monster & dummy();
	Monster & player();
};

struct GameWithDummyAndKiller {
	DummyDungeon game;
	GameWithDummyAndKiller();
	Monster & dummy();
	Monster & killer();
	Monster & poisoner();
};

struct Game2x2 {
	DummyDungeon game;
	Game2x2();
};

struct GameWithLevels {
	TestDungeon game;
	GameWithLevels(): game(Point(1, 1), Point(2, 2)) {}
};

struct LevelWithPath {
	DummyDungeon game;
	LevelWithPath();
};

struct LevelForSeeing {
	DummyDungeon game;
	LevelForSeeing();
};

}

}
