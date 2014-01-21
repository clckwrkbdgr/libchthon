#pragma once
#include <map>
#include <vector>

namespace Chthon { /// @defgroup AI AI
/// @{

class Game;
class Monster;
class Action;

/** Basic controller class. Represents monster behaviour.
 * Subclass it and override pure virtual act() function in order to define new behaviour.
 */
class Controller {
public:
	virtual ~Controller() {}
	/** Produces some Action basing on its state and game environment.
	 * Note that this function should just construct Action, which then must be commited.
	 */
	virtual Action * act(Monster &, Game &) = 0;
};

/** Simple AI with some basic, not very wit monster needs - walk, attack on sight etc.
 *
 * AI is constructed using chain of commands. If first command wasn't succeed, go to the next one, and so one.
 * Last command is always WAIT.
 * @code{.cpp}
 * BasicAI * ai = new BasicAI();
 * ai->add(MOVE_TO_HIT_PLAYER_IF_SEES)->add(MOVE_RANDOM)->add(WAIT);
 * @endcode
 */
class BasicAI : public Controller {
public:
	enum {
		WAIT, ///< Do nothing. Always succeeds.
		MOVE_TO_HIT_PLAYER_IF_SEES, ///< If player monster is in sight range, move to it, otherwise fail.
		HIT_PLAYER_IF_NEAR, ///< If player is in the reach, hit it. Otherwise fail.
		MOVE_RANDOM, ///< Move in random direction. Always succeeds.
		COUNT
	};
	virtual ~BasicAI() {}
	/// Adds next command to the chain.
	BasicAI * add(unsigned action);
	/// Produces action using set chain of commands.
	virtual Action * act(Monster & monster, Game & game);
private:
	std::vector<unsigned> actions;
};

/// Stores controllers by its id.
struct ControllerFactory {
	~ControllerFactory();
	/// Add new controller under given id.
	void add_controller(int ai, Controller * controller);
	/// Get controller by its id.
	Controller * get_controller(int ai) const;
private:
	std::map<int, Controller*> controllers;
};

/// @}
}
