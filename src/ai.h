#pragma once
#include <map>
#include <vector>

namespace Chthon {

class Game;
class Monster;
class Action;

class Controller {
public:
	virtual ~Controller() {}
	virtual Action * act(Monster &, Game &) = 0;
};

class BasicAI : public Controller {
public:
	enum {
		WAIT, MOVE_TO_HIT_PLAYER_IF_SEES, HIT_PLAYER_IF_NEAR, MOVE_RANDOM,
		COUNT
	};
	virtual ~BasicAI() {}
	BasicAI * add(unsigned action);
	virtual Action * act(Monster & monster, Game & game);
private:
	std::vector<unsigned> actions;
};

struct ControllerFactory {
	~ControllerFactory();
	void add_controller(int ai, Controller * controller);
	Controller * get_controller(int ai) const;
private:
	std::map<int, Controller*> controllers;
};

}
