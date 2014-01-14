#include "actions.h"
#include "monsters.h"
#include "game.h"

namespace Chthon {

void Move::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level().monsters, new_pos);
	if(monster.valid()) {
		game.event(someone, GameEvent::BUMPS_INTO, monster);
		return;
	}
    Object & object = find_at(game.level().objects, new_pos);
	if(object.valid() && !object.type->passable) {
		game.event(someone, GameEvent::BUMPS_INTO, object);
		return;
	}
	if(!game.level().cell_type_at(new_pos).passable) {
		game.event(someone, GameEvent::BUMPS_INTO, game.level().cell_type_at(new_pos));
		return;
	}
    someone.pos = new_pos;
}

void Drink::commit(Monster & someone, Game & game)
{
	Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level().monsters, new_pos);
	assert(!monster.valid(), Exception::CANNOT_DRINK, someone, monster);
    Object & object = find_at(game.level().objects, new_pos);
	assert(object.valid() && object.type->drinkable, Exception::NOTHING_TO_DRINK, someone, object);
	game.event(someone, GameEvent::DRINKS, object);
	if(someone.heal_by(1)) {
		game.event(object, GameEvent::HEALS, someone);
	}
}

void Open::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level().objects, new_pos);
	assert(object.valid() && (object.type->openable || object.type->containable), Exception::NOTHING_TO_OPEN, someone);
    if(object.type->openable) {
		assert(!object.opened(), Exception::ALREADY_OPENED, object);
		if(object.locked) {
			assert(someone.inventory.has_key(object.lock_type), Exception::LOCKED, object);
			game.event(someone, GameEvent::UNLOCKS, object);
			object.locked = false;
		}
		object.open();
		game.event(someone, GameEvent::OPENS, object);
    } else if(object.type->containable) {
		assert(!object.items.empty(), Exception::HAS_NO_ITEMS, object);
		foreach(Item & item, object.items) {
			item.pos = someone.pos;
			game.level().items.push_back(item);
			game.event(someone, GameEvent::TAKES_FROM, item, object);
		}
		object.items.clear();
	}
}

void Close::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Object & object = find_at(game.level().objects, new_pos);
    assert(object.valid() && object.type->openable, Exception::NOTHING_TO_CLOSE, someone);
    assert(object.opened(), Exception::ALREADY_CLOSED, object);
    object.close();
    game.event(someone, GameEvent::CLOSES, object);
}

void Swing::commit(Monster & someone, Game & game)
{
    Point new_pos = someone.pos + shift;
    Monster & monster = find_at(game.level().monsters, new_pos);
	if(monster.valid()) {
		game.hit(someone, monster, someone.damage());
		return;
	}
    Object & object = find_at(game.level().objects, new_pos);
	if(object.valid()) {
		game.event(someone, GameEvent::HITS, object);
		if(object.type->openable && !object.opened()) {
			if(object.locked) {
				assert(someone.inventory.has_key(object.lock_type), Exception::LOCKED, object);
				game.event(someone, GameEvent::UNLOCKS, object);
				object.locked = false;
			}
			object.open();
			game.event(someone, GameEvent::OPENS, object);
		}
		return;
	}
	if(!game.level().cell_type_at(new_pos).passable) {
		game.event(someone, GameEvent::HITS, game.level().cell_type_at(new_pos));
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
		if(!game.level().map.valid(item.pos)) {
			break;
		}
		if(!game.level().cell_type_at(item.pos + shift).transparent) {
			game.event(item, GameEvent::HITS, game.level().cell_type_at(item.pos + shift));
			game.level().items.push_back(item);
			break;
		}
		Object & object = find_at(game.level().objects, item.pos + shift);
		if(object.valid()) {
			if(object.type->containable) {
				game.event(item, GameEvent::FALLS_INTO, object);
				object.items.push_back(item);
				break;
			} else if(object.type->drinkable) {
				game.event(item, GameEvent::FALLS_INTO, object);
				break;
			} else if(!object.type->transparent) {
				game.event(item, GameEvent::HITS, object);
				game.level().items.push_back(item);
				break;
			}
		}
		Monster & monster = find_at(game.level().monsters, item.pos + shift);
		if(monster.valid()) {
			item.pos += shift;
			game.level().items.push_back(item);
			game.hit(item, monster, item.type->damage);
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

	Object & object = find_at(game.level().objects, item.pos + shift);
	if(object.valid() && object.type->drinkable && item.is_emptyable()) {
		assert(item.is_empty(), Exception::ALREADY_FULL, item);
		game.event(someone, GameEvent::REFILLS, item);
		item.make_full();
		return;
	}
	
	if(game.level().cell_type_at(item.pos + shift).passable) {
		item.pos += shift;
	}
	game.event(someone, GameEvent::DROPS_AT, item, game.level().cell_type_at(item.pos));
	game.level().items.push_back(item);
	someone.inventory.take_wielded_item();
}


}
