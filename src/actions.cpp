#include "actions.h"
#include "game.h"
#include "objects.h"
#include "monsters.h"
#include "cell.h"
#include "util.h"
#include "format.h"

namespace Chthon {

void Action::assert(bool expression, GameEvent::EventType exception_type, const Info & subject, const Info & object)
{
	if(!expression) {
		throw Exception(subject, exception_type, object);
	}
}


void Drop::commit(Monster & someone, Game & game)
{
	assert(!someone.inventory.empty(), Exception::NOTHING_TO_DROP, someone);
	assert(someone.inventory.get_item(slot).valid(), Exception::NO_SUCH_ITEM, someone);
	if(someone.inventory.wields(slot)) {
		game.event(someone, GameEvent::UNWIELDS, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.wears(slot)) {
		game.event(someone, GameEvent::TAKES_OFF, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	Item item = someone.inventory.take_item(slot);
	item.pos = someone.pos;
	game.current_level().items.push_back(item);
	game.event(someone, GameEvent::DROPS_AT, item, game.current_level().cell_type_at(someone.pos));
}

void Grab::commit(Monster & someone, Game & game)
{
	auto & items = game.current_level().items;
	auto item = std::find_if(items.begin(), items.end(),
			[someone](const Item & i) { return i.pos == someone.pos; }
			);
	assert(item != items.end(), Exception::NOTHING_TO_GRAB, someone);
	unsigned slot = someone.inventory.insert(*item);
	assert(slot != Inventory::NOTHING, Exception::NO_SPACE_LEFT, someone);
	game.event(someone, GameEvent::PICKS_UP_FROM, *item, game.current_level().cell_type_at(someone.pos));
	if(item->type->quest) {
		game.event(someone, GameEvent::PICKED_UP_A_QUEST_ITEM);
	}
	game.current_level().items.erase(item);
}

void Wield::commit(Monster & someone, Game & game)
{
	assert(!someone.inventory.empty(), Exception::NOTHING_TO_WIELD, someone);
	assert(someone.inventory.get_item(slot).valid(), Exception::NO_SUCH_ITEM, someone);
	if(someone.inventory.wielded_item().valid()) {
		game.event(someone, GameEvent::UNWIELDS, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.wears(slot)) {
		game.event(someone, GameEvent::TAKES_OFF, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	someone.inventory.wield(slot);
	game.event(someone, GameEvent::WIELDS, someone.inventory.wielded_item());
}

void Unwield::commit(Monster & someone, Game & game)
{
	const Item & item = someone.inventory.wielded_item();
	assert(item.valid(), Exception::NOTHING_TO_UNWIELD, someone);
	game.event(someone, GameEvent::UNWIELDS, item);
	someone.inventory.unwield();
}

void Wear::commit(Monster & someone, Game & game)
{
	assert(!someone.inventory.empty(), Exception::NOTHING_TO_WEAR, someone);
	const Item & item = someone.inventory.get_item(slot);
	assert(item.valid(), Exception::NO_SUCH_ITEM, someone);
	assert(item.type->wearable, Exception::CANNOT_WEAR, someone, item);
	if(someone.inventory.wields(slot)) {
		game.event(someone, GameEvent::UNWIELDS, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	if(someone.inventory.worn_item().valid()) {
		game.event(someone, GameEvent::TAKES_OFF, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	someone.inventory.wear(slot);
	game.event(someone, GameEvent::WEARS, item);
}

void TakeOff::commit(Monster & someone, Game & game)
{
	const Item & item = someone.inventory.worn_item();
	assert(item.valid(), Exception::NOTHING_TO_TAKE_OFF, someone);
	game.event(someone, GameEvent::TAKES_OFF, item);
	someone.inventory.unwield();
}

void Eat::commit(Monster & someone, Game & game)
{
	assert(!someone.inventory.empty(), Exception::NOTHING_TO_EAT, someone);
	Item & item = someone.inventory.get_item(slot);
	assert(item.valid(), Exception::NO_SUCH_ITEM, someone);
	assert(item.type->edible, Exception::CANNOT_EAT, someone, item);
	if(someone.inventory.wears(slot)) {
		game.event(someone, GameEvent::TAKES_OFF, someone.inventory.worn_item());
		someone.inventory.take_off();
	}
	if(!item.is_emptyable() && someone.inventory.wields(slot)) {
		game.event(someone, GameEvent::UNWIELDS, someone.inventory.wielded_item());
		someone.inventory.unwield();
	}
	game.event(someone, GameEvent::EATS, item);
	if(item.type->antidote > 0 && someone.poisoning > 0) {
		someone.poisoning -= item.type->antidote;
		someone.poisoning = std::max(0, someone.poisoning);
		game.event(item, GameEvent::CURES_POISONING, someone);
	}
	if(item.type->healing > 0) {
		if(someone.heal_by(item.type->healing)) {
			game.event(item, GameEvent::HEALS, someone);
		}
	}
	if(item.is_emptyable()) {
		game.event(someone, GameEvent::EMPTIES, item);
		item.make_empty();
	} else {
		someone.inventory.take_item(slot);
	}
}

void GoUp::commit(Monster & someone, Game & game)
{
	auto & objects = game.current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[someone](const Object & i) { return i.pos == someone.pos; }
			);
	assert(object != objects.end() && object->type->transporting && object->up_destination, Exception::CANNOT_GO_UP, someone);
	if(object->is_exit_up()) {
		const Item & quest_item = someone.inventory.quest_item();
		if(quest_item.valid()) {
			game.event(someone, GameEvent::WINS_GAME_WITH, quest_item);
			game.state = Game::COMPLETED;
		} else {
			game.event(someone, GameEvent::SHOULD_GET_QUEST_ITEM);
		}
	} else {
		game.event(someone, GameEvent::GOES_UP);
		game.go_to_level(object->up_destination);
		game.state = Game::TURN_ENDED;
	}
}

void GoDown::commit(Monster & someone, Game & game)
{
	auto & objects = game.current_level().objects;
	auto object = std::find_if(objects.begin(), objects.end(),
			[someone](const Object & i) { return i.pos == someone.pos; }
			);
	assert(object != objects.end() && object->type->transporting && object->down_destination, Exception::CANNOT_GO_UP, someone);
	if(object->is_exit_down()) {
		const Item & quest_item = someone.inventory.quest_item();
		if(quest_item.valid()) {
			game.event(someone, GameEvent::WINS_GAME_WITH, quest_item);
			game.state = Game::COMPLETED;
		} else {
			game.event(someone, GameEvent::SHOULD_GET_QUEST_ITEM);
		}
	} else {
		game.event(someone, GameEvent::GOES_DOWN);
		game.go_to_level(object->down_destination);
		game.state = Game::TURN_ENDED;
	}
}

}
