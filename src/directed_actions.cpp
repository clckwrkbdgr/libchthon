#include "actions.h"
#include "monsters.h"
#include "game.h"
#include "log.h"

namespace Chthon {

void Move::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
	auto & monsters = game.current_level().monsters;
	auto monster = std::find_if(monsters.begin(), monsters.end(),
			[new_pos](const Monster & m) { return m.pos == new_pos; }
			);
	if(monster != monsters.end()) {
		game.event(someone, GameEvent::BUMPS_INTO, *monster);
		return;
	}
	auto & objects = game.current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[new_pos](const Object & m) { return m.pos == new_pos; }
			);
	if(object != objects.end() && !object->type->passable) {
		game.event(someone, GameEvent::BUMPS_INTO, *object);
		return;
	}
	if(!game.current_level().cell_type_at(new_pos).passable) {
		game.event(someone, GameEvent::BUMPS_INTO, game.current_level().cell_type_at(new_pos));
		return;
	}
    someone.pos = new_pos;
}

static const Monster & default_value(std::vector<Monster>::const_iterator it, std::vector<Monster>::const_iterator end)
{
	if(it != end) {
		return *it;
	}
	static Monster empty;
	return empty;
}

void Drink::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
	auto & monsters = game.current_level().monsters;
	auto monster = std::find_if(monsters.begin(), monsters.end(),
			[new_pos](const Monster & m) { return m.pos == new_pos; }
			);
	assert(monster == monsters.end(), Exception::CANNOT_DRINK, someone, default_value(monster, monsters.end()));
	auto & objects = game.current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[new_pos](const Object & m) { return m.pos == new_pos; }
			);
	assert(object != objects.end() && object->type->drinkable, Exception::NOTHING_TO_DRINK, someone);
	game.event(someone, GameEvent::DRINKS, *object);
	if(someone.heal_by(1)) {
		game.event(*object, GameEvent::HEALS, someone);
	}
}

void Open::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
	auto & objects = game.current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[new_pos](const Object & m) { return m.pos == new_pos; }
			);
	assert(object != objects.end() && (object->type->openable || object->type->containable), Exception::NOTHING_TO_OPEN, someone);
    if(object->type->openable) {
		assert(!object->opened(), Exception::ALREADY_OPENED, *object);
		if(object->locked) {
			assert(someone.inventory.has_key(object->lock_type), Exception::LOCKED, *object);
			game.event(someone, GameEvent::UNLOCKS, *object);
			object->locked = false;
		}
		object->open();
		game.event(someone, GameEvent::OPENS, *object);
    } else if(object->type->containable) {
		assert(!object->items.empty(), Exception::HAS_NO_ITEMS, *object);
		foreach(Item & item, object->items) {
			item.pos = someone.pos;
			game.current_level().items.push_back(item);
			game.event(someone, GameEvent::TAKES_FROM, item, *object);
		}
		object->items.clear();
	}
}

void Close::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
	auto & objects = game.current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[new_pos](const Object & m) { return m.pos == new_pos; }
			);
    assert(object != objects.end() && object->type->openable, Exception::NOTHING_TO_CLOSE, someone);
    assert(object->opened(), Exception::ALREADY_CLOSED, *object);
    object->close();
    game.event(someone, GameEvent::CLOSES, *object);
}

void Swing::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
	auto & monsters = game.current_level().monsters;
	auto monster = std::find_if(monsters.begin(), monsters.end(),
			[new_pos](const Monster & m) { return m.pos == new_pos; }
			);
	if(monster != monsters.end()) {
		game.hit(someone, *monster, someone.damage());
		return;
	}
	auto & objects = game.current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[new_pos](const Object & m) { return m.pos == new_pos; }
			);
	if(object != objects.end()) {
		game.event(someone, GameEvent::HITS, *object);
		if(object->type->openable && !object->opened()) {
			if(object->locked) {
				assert(someone.inventory.has_key(object->lock_type), Exception::LOCKED, *object);
				game.event(someone, GameEvent::UNLOCKS, *object);
				object->locked = false;
			}
			object->open();
			game.event(someone, GameEvent::OPENS, *object);
		}
		return;
	}
	if(!game.current_level().cell_type_at(new_pos).passable) {
		game.event(someone, GameEvent::HITS, game.current_level().cell_type_at(new_pos));
		return;
	}
    game.event(someone, GameEvent::SWINGS_AT_NOTHING);
}

void Fire::commit(Monster & someone, Game & game)
{
	Item item = someone.inventory.take_wielded_item();
	assert(item.valid(), Exception::NOTHING_TO_THROW, someone);
    item.pos = someone.pos;
	game.event(someone, GameEvent::THROWS, item);
	while(true) {
		if(!game.current_level().map.valid(item.pos + shift)) {
			break;
		}
		if(!game.current_level().cell_type_at(item.pos + shift).transparent) {
			game.event(item, GameEvent::HITS, game.current_level().cell_type_at(item.pos + shift));
			game.current_level().items.push_back(item);
			break;
		}
		Point new_pos = item.pos + shift;
		auto & objects = game.current_level().objects;
		auto object = std::find_if(objects.begin(), objects.end(),
				[new_pos](const Object & m) { return m.pos == new_pos; }
				);
		if(object != objects.end()) {
			if(object->type->containable) {
				game.event(item, GameEvent::FALLS_INTO, *object);
				object->items.push_back(item);
				break;
			} else if(object->type->drinkable) {
				game.event(item, GameEvent::FALLS_INTO, *object);
				break;
			} else if(!object->type->transparent) {
				game.event(item, GameEvent::HITS, *object);
				game.current_level().items.push_back(item);
				break;
			}
		}
		auto & monsters = game.current_level().monsters;
		auto monster = std::find_if(monsters.begin(), monsters.end(),
				[new_pos](const Monster & m) { return m.pos == new_pos; }
				);
		if(monster != monsters.end()) {
			item.pos += shift;
			game.current_level().items.push_back(item);
			game.hit(item, *monster, item.type->damage);
			break;
		}
		item.pos += shift;
	}
}

void Put::commit(Monster & someone, Game & game)
{
	Item & item = someone.inventory.wielded_item();
	assert(item.valid(), Exception::NOTHING_TO_PUT, someone);

	item.pos = someone.pos;
	Point new_pos = item.pos + shift;
	auto & objects = game.current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[new_pos](const Object & m) { return m.pos == new_pos; }
			);
	if(object != objects.end() && object->type->drinkable && item.is_emptyable()) {
		assert(item.is_empty(), Exception::ALREADY_FULL, item);
		game.event(someone, GameEvent::REFILLS, item);
		item.make_full();
		return;
	}
	
	if(game.current_level().cell_type_at(item.pos + shift).passable) {
		item.pos += shift;
	}
	game.event(someone, GameEvent::DROPS_AT, item, game.current_level().cell_type_at(item.pos));
	game.current_level().items.push_back(item);
	someone.inventory.take_wielded_item();
}


}
