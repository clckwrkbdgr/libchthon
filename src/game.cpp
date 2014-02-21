#include "game.h"
#include "actions.h"
#include "monsters.h"
#include "format.h"
#include "log.h"
#include <map>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <cmath>

namespace Chthon {

GameEvent::GameEvent(const Info & event_actor, EventType event_type, const Info & event_target, const Info & event_help)
	: type(event_type), amount(0), actor(event_actor), target(event_target), help(event_help)
{
}

GameEvent::GameEvent(const Info & event_actor, EventType event_type, int event_amount, const Info & event_target, const Info & event_help)
	: type(event_type), amount(event_amount), actor(event_actor), target(event_target), help(event_help)
{
}

std::string to_string(const GameEvent & e)
{
	return format("<{1}> do #{0} ({2}) on <{3}> with <{4}>", e.type, e.actor.id, e.amount, e.target.id, e.help.id);
}


Game::Game()
	: state(PLAYING), turns(0), current_level_index(0)
{
}

Game::~Game()
{
}

void Game::create_new_game()
{
	go_to_level(1);
}

Level & Game::current_level()
{
	return levels[current_level_index];
}

const Level & Game::current_level() const
{
	return levels.find(current_level_index)->second;
}

void Game::go_to_level(int level_index)
{
	Monster player = current_level().get_player();

	current_level_index = level_index;
	if(levels.count(level_index) == 0) {
		generate(current_level(), current_level_index);
	}
	if(player.valid()) {
		player.pos = current_level().get_player().pos;
		current_level().get_player() = player;
	} else {
		log("Player wasn't found on the level when travelling!");
	}
}

const ItemType * Game::item_type(const std::string & id) const
{
	return get_pointer(item_types, id);
}

const ObjectType * Game::object_type(const std::string & id) const
{
	return get_pointer(object_types, id);
}

const MonsterType * Game::monster_type(const std::string & id) const
{
	return get_pointer(monster_types, id);
}

const CellType * Game::cell_type(const std::string & id) const
{
	return get_pointer(cell_types, id);
}

ItemType::Builder Game::add_item_type(const std::string & id)
{
	return insert_builder(item_types, id);
}

ObjectType::Builder Game::add_object_type(const std::string & id)
{
	return insert_builder(object_types, id);
}

MonsterType::Builder Game::add_monster_type(const std::string & id)
{
	return insert_builder(monster_types, id);
}

CellType::Builder Game::add_cell_type(const std::string & id)
{
	return insert_builder(cell_types, id);
}

Item::Builder Game::add_item(Level & level, const std::string & type_id)
{
	level.items.push_back(Item(get_pointer(item_types, type_id)));
	return Item::Builder(level.items.back());
}

Item::Builder Game::add_item(Level & level, const std::string & full_type_id, const std::string & empty_type_id)
{
	level.items.push_back(Item(get_pointer(item_types, full_type_id), get_pointer(item_types, empty_type_id)));
	return Item::Builder(level.items.back());
}

Object::Builder Game::add_object(Level & level, const std::string & type_id)
{
	level.objects.push_back(Object(get_pointer(object_types, type_id)));
	return Object::Builder(level.objects.back());
}

Object::Builder Game::add_object(Level & level, const std::string & closed_type_id, const std::string & opened_type_id)
{
	level.objects.push_back(Object(get_pointer(object_types, closed_type_id), get_pointer(object_types, opened_type_id)));
	return Object::Builder(level.objects.back());
}

Monster::Builder Game::add_monster(Level & level, const std::string & type_id)
{
	level.monsters.push_back(Monster(get_pointer(monster_types, type_id)));
	return Monster::Builder(level.monsters.back());
}

Item::Builder Game::add_item(const std::string & type_id)
{
	return add_item(current_level(), type_id);
}

Item::Builder Game::add_item(const std::string & full_type_id, const std::string & empty_type_id)
{
	return add_item(current_level(), full_type_id, empty_type_id);
}

Object::Builder Game::add_object(const std::string & type_id)
{
	return add_object(current_level(), type_id);
}

Object::Builder Game::add_object(const std::string & closed_type_id, const std::string & opened_type_id)
{
	return add_object(current_level(), closed_type_id, opened_type_id);
}

Monster::Builder Game::add_monster(const std::string & type_id)
{
	return add_monster(current_level(), type_id);
}


void Game::run()
{
	state = PLAYING;
	while(state == PLAYING) {
		foreach(Monster & monster, current_level().monsters) {
			if(monster.is_dead()) {
				continue;
			}
			Controller * controller = controller_factory.get_controller(deref_default(monster.type).ai);
			if(!controller) {
				log("No controller found for AI #{0}!", deref_default(monster.type).ai);
				continue;
			}
			current_level().invalidate_fov(monster);
			Action * action = controller->act(monster, *this);
			if(action) {
				try {
					action->commit(monster, *this);
				} catch(const Action::Exception & e) {
					events.push_back(e);
				}
				delete action;
				action = nullptr;
			}
			if(state == TURN_ENDED) {
				break;
			}

			process_environment(monster);

			if(state != PLAYING) {
				break;
			}
		}
		current_level().erase_dead_monsters();
		++turns;
		if(state == TURN_ENDED) {
			state = PLAYING;
		}
	}
}

void Game::event(const GameEvent & e)
{
	events.push_back(e);
}

void Game::event(const Info & event_actor, GameEvent::EventType event_type, const Info & event_target, const Info & event_help)
{
	events.push_back(GameEvent(event_actor, event_type, event_target, event_help));
}

void Game::event(const Info & event_actor, GameEvent::EventType event_type, int event_amount, const Info & event_target, const Info & event_help)
{
	events.push_back(GameEvent(event_actor, event_type, event_amount, event_target, event_help));
}

void Game::process_environment(Monster & someone)
{
	if(current_level().cell_type_at(someone.pos).hurts) {
		event(current_level().cell_type_at(someone.pos), GameEvent::HURTS, someone);
		hurt(someone, 1);
	}
	auto & objects = current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[someone](const Object & i) { return i.pos == someone.pos; }
			);
	if(object != objects.end() && deref_default(object->type).triggerable) {
		if(object->items.empty()) {
			event(*object, GameEvent::TRAP_IS_OUT_OF_ITEMS);
		} else {
			event(someone, GameEvent::TRIGGERS, *object);
			object->items.back().pos = object->pos;
			current_level().items.push_back(object->items.back());
			object->items.pop_back();
			hurt(someone, 1);
		}
	}
	if(someone.poisoning > 0) {
		event(someone, GameEvent::IS_HURT_BY_POISONING);
		--someone.poisoning;
		hurt(someone, 1, true);
	} else if(someone.poisoning < 0) {
		someone.poisoning = 0;
	}
}

void Game::die(Monster & someone)
{
	Item item;
	while((item = someone.inventory.take_first_item()).valid()) {
		item.pos = someone.pos;
		current_level().items.push_back(item);
		event(someone, GameEvent::DROPS_AT, item, current_level().cell_type_at(someone.pos));
	}
	event(someone, GameEvent::DIED);
	if(deref_default(someone.type).faction == Monster::PLAYER) {
		state = PLAYER_DIED;
	}
}

void Game::hurt(Monster & someone, int damage, bool pierce_armour)
{
	int received_damage = damage - (pierce_armour ? 0 : deref_default(someone.inventory.worn_item().type).defence);
	someone.hp -= received_damage;
	event(someone, GameEvent::LOSES_HEALTH, received_damage);
	if(someone.is_dead()) {
		die(someone);
	}
}

void Game::hit(Item & item, Monster & other, int damage)
{
	int received_damage = damage - deref_default(other.inventory.worn_item().type).defence;
	other.hp -= received_damage;
	event(item, GameEvent::HITS_FOR_HEALTH, received_damage, other);
	if(other.is_dead()) {
		die(other);
	}
}

void Game::hit(Monster & someone, Monster & other, int damage)
{
	int received_damage = damage - deref_default(other.inventory.worn_item().type).defence;
	other.hp -= received_damage;
	event(someone, GameEvent::HITS_FOR_HEALTH, received_damage, other);
	if(deref_default(someone.type).poisonous) {
		event(someone, GameEvent::POISONS, other);
		other.poisoning = std::min(5, std::max(5, other.poisoning));
	}
	if(other.is_dead()) {
		die(other);
	}
}


}
