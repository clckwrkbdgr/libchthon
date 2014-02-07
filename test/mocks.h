#pragma once
#include "../src/level.h"
#include "../src/game.h"

namespace GameMocks {

struct DummyDungeon : public Chthon::Game {
	DummyDungeon();
	virtual ~DummyDungeon();
	virtual void generate(Chthon::Level & /*level*/, int /*level_index*/);
	virtual void create_types(Chthon::Game & /*game*/);
};

class TestDungeon : public Chthon::Game {
public:
	TestDungeon(const Chthon::Point & player_pos1, const Chthon::Point & player_pos2);
	virtual void create_types(Chthon::Game & game);
	virtual void generate(Chthon::Level & level, int level_index);
	bool was_generated() const { return generated; }
private:
	bool generated;
	Chthon::Point pos1, pos2;
	Chthon::MonsterType player_one;
	Chthon::MonsterType player_two;
};


struct GameWithDummyWieldingAndWearing {
	DummyDungeon game;
	GameWithDummyWieldingAndWearing();
	Chthon::Monster & dummy();
};

struct GameWithDummyWithItems {
	DummyDungeon game;
	GameWithDummyWithItems();
	Chthon::Monster & dummy();
};

struct GameWithDummyAndFood {
	DummyDungeon game;
	enum { ARMOR, SPEAR, JUNK, FOOD, MEDKIT, MEGASPHERE, ANTIDOTE, FULL_FLASK, EMPTY_FLASK, EMPTY, NONE };
	GameWithDummyAndFood();
	Chthon::Monster & dummy();
};

struct GameWithDummyAndStairs {
	TestDungeon game;
	GameWithDummyAndStairs();
	Chthon::Monster & dummy();
	Chthon::Object & stairs();
};

struct GameWithDummyAndObjects {
	DummyDungeon game;
	GameWithDummyAndObjects();
	Chthon::Monster & dummy();
};

struct GameWithDummyOnTrap {
	DummyDungeon game;
	GameWithDummyOnTrap();
	Chthon::Monster & dummy();
};

struct GameWithDummy {
	DummyDungeon game;
	GameWithDummy();
	Chthon::Monster & dummy();
	Chthon::Monster & player();
};

struct GameWithDummyAndKiller {
	DummyDungeon game;
	GameWithDummyAndKiller();
	Chthon::Monster & dummy();
	Chthon::Monster & killer();
	Chthon::Monster & poisoner();
};

struct Game2x2 {
	DummyDungeon game;
	Game2x2();
};

struct GameWithLevels {
	TestDungeon game;
	GameWithLevels(): game(Chthon::Point(1, 1), Chthon::Point(2, 2)) {}
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
