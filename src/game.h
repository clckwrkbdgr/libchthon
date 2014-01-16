#pragma once
#include "map.h"
#include "objects.h"
#include "monsters.h"
#include "ai.h"
#include "level.h"
#include "info.h"
#include <map>
#include <list>

namespace Chthon {

struct GameEvent {
	enum EventType {
		UNKNOWN,

		CURES_POISONING, HEALS, HURTS, IS_HURT_BY_POISONING, LOSES_HEALTH, DIED,
		HITS, HITS_FOR_HEALTH, BUMPS_INTO, POISONS, SWINGS_AT_NOTHING,
		OPENS, CLOSES, DRINKS, GOES_DOWN, GOES_UP, UNLOCKS,
		TRAP_IS_OUT_OF_ITEMS, TRIGGERS,
		EATS, EMPTIES, REFILLS, TAKES_OFF, THROWS, UNWIELDS, WEARS, WIELDS,
		DROPS_AT, FALLS_INTO, PICKS_UP_FROM, TAKES_FROM,
		PICKED_UP_A_QUEST_ITEM, SHOULD_GET_QUEST_ITEM, WINS_GAME_WITH,

		ALREADY_CLOSED, ALREADY_FULL, ALREADY_OPENED,
		CANNOT_DRINK, CANNOT_EAT, CANNOT_GO_DOWN, CANNOT_GO_UP, CANNOT_WEAR,
		LOCKED,
		NOTHING_TO_CLOSE, NOTHING_TO_DRINK, NOTHING_TO_DROP, NOTHING_TO_EAT, NOTHING_TO_GRAB, NOTHING_TO_OPEN,
		NOTHING_TO_TAKE_OFF, NOTHING_TO_UNWIELD, NOTHING_TO_WEAR, NOTHING_TO_WIELD, NOTHING_TO_PUT, NOTHING_TO_THROW,
		NO_SPACE_LEFT, NO_SUCH_ITEM, HAS_NO_ITEMS,

		COUNT
	};

	EventType type;
	int amount;
	Info actor, target, help;
	GameEvent(const Info & event_actor, EventType event_type, const Info & event_target = Info(), const Info & event_help = Info());
	GameEvent(const Info & event_actor, EventType event_type, int event_amount, const Info & event_target = Info(), const Info & event_help = Info());
};
std::string to_string(const GameEvent & e);

struct Game {
	enum State { PLAYING, TURN_ENDED, SUSPENDED, PLAYER_DIED, COMPLETED };
	State state;
	int turns;
	std::vector<GameEvent> events;
	int current_level_index;
	std::map<int, Level> levels;

	TypeRegistry<std::string, Cell> cell_types;
	TypeRegistry<std::string, Monster> monster_types;
	TypeRegistry<std::string, Object> object_types;
	TypeRegistry<std::string, Item> item_types;
	ControllerFactory controller_factory;

	Game();
	virtual ~Game();
	void create_new_game();
	void run();
	virtual void generate(Level & level, int level_index) = 0;

	Level & current_level();
	const Level & current_level() const;
	void go_to_level(int level);

	void event(const GameEvent & e);
	void event(const Info & event_actor, GameEvent::EventType event_type, const Info & event_target = Info(), const Info & event_help = Info());
	void event(const Info & event_actor, GameEvent::EventType event_type, int event_amount, const Info & event_target = Info(), const Info & event_help = Info());

	const ItemType * item_type(const std::string & id) const;
	const ObjectType * object_type(const std::string & id) const;
	const MonsterType * monster_type(const std::string & id) const;
	const CellType * cell_type(const std::string & id) const;

	ItemType::Builder add_item_type(const std::string & id);
	ObjectType::Builder add_object_type(const std::string & id);
	MonsterType::Builder add_monster_type(const std::string & id);
	CellType::Builder add_cell_type(const std::string & id);

	Item::Builder add_item(Level & level, const std::string & type_id);
	Item::Builder add_item(Level & level, const std::string & full_type_id, const std::string & empty_type_id);
	Object::Builder add_object(Level & level, const std::string & type_id);
	Object::Builder add_object(Level & level, const std::string & closed_type_id, const std::string & opened_type_id);
	Monster::Builder add_monster(Level & level, const std::string & type_id);
	Item::Builder add_item(const std::string & type_id);
	Item::Builder add_item(const std::string & full_type_id, const std::string & empty_type_id);
	Object::Builder add_object(const std::string & type_id);
	Object::Builder add_object(const std::string & closed_type_id, const std::string & opened_type_id);
	Monster::Builder add_monster(const std::string & type_id);

	void process_environment(Monster & someone);
	void die(Monster & someone);
	void hurt(Monster & someone, int damage, bool pierce_armour = false);
	void hit(Monster & someone, Monster & other, int damage);
	void hit(Item & item, Monster & other, int damage);
};

}
