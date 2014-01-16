#include "ai.h"
#include "game.h"
#include "actions.h"
#include "log.h"
#include "util.h"
#include <cstdlib>

namespace Chthon {

BasicAI * BasicAI::add(unsigned action)
{
	if(action < COUNT) {
		actions.push_back(action);
	} else {
		log("Unknown basic AI action: {0}", action);
	}
	return this;
}

Action * BasicAI::act(Monster & monster, Game & game)
{
	const Monster & player = game.current_level().get_player();
	bool sees_player = game.current_level().map.cell(player.pos).visible;
	int d = distance(monster.pos, player.pos);
	Point shift = Point(
			sign(player.pos.x - monster.pos.x),
			sign(player.pos.y - monster.pos.y)
			);
	Point random_direction = Point(rand() % 3 - 1, rand() % 3 - 1);

	foreach(unsigned action, actions) {
		switch(action) {
			case MOVE_TO_HIT_PLAYER_IF_SEES:
				if(sees_player && 1 < d && d <= monster.type->sight) {
					return new Move(shift);
				} else if(sees_player && d == 1) {
					return new Swing(shift);
				}
				break;
			case HIT_PLAYER_IF_NEAR:
				if(sees_player && d == 1) {
					return new Swing(shift);
				}
				break;
			case MOVE_RANDOM:
				if(!random_direction.null()) {
					return new Move(random_direction);
				}
				break;
			default:
			case WAIT: return new Wait();
		};
	}
	return new Wait();
}


ControllerFactory::~ControllerFactory()
{
	typedef std::pair<const int, Controller*> KeyValue;
	foreach(KeyValue & key_value, controllers) {
		delete key_value.second;
	}
}

void ControllerFactory::add_controller(int ai, Controller * controller)
{
	controllers[ai] = controller;
}

Controller * ControllerFactory::get_controller(int ai) const
{
	std::map<int, Controller*>::const_iterator result = controllers.find(ai);
	if(result != controllers.end()) {
		return result->second;
	}
	log("Unknown AI code: {0}", ai);
	return nullptr;
}

}
