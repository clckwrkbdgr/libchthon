#include "items.h"
#include "util.h"

namespace Chthon {

ItemType::ItemType(const std::string & type_id)
	: id(type_id), sprite(0), damage(0), wearable(false), defence(0), edible(false), antidote(0), healing(0), quest(false)
{
}

ItemType::Builder & ItemType::Builder::sprite(const int & value) { result.sprite = value; return *this; }
ItemType::Builder & ItemType::Builder::name(const std::string & value) { result.name = value; return *this; }
ItemType::Builder & ItemType::Builder::damage(int value) { result.damage = value; return *this; }
ItemType::Builder & ItemType::Builder::wearable() { result.wearable = true; return *this; }
ItemType::Builder & ItemType::Builder::defence(int value) { result.defence = value; return *this; }
ItemType::Builder & ItemType::Builder::edible() { result.edible = true; return *this; }
ItemType::Builder & ItemType::Builder::antidote(int value) { result.antidote = value; return *this; }
ItemType::Builder & ItemType::Builder::healing(int value) { result.healing = value; return *this; }
ItemType::Builder & ItemType::Builder::quest() { result.quest = true; return *this; }


Item::Item(const Type * item_type)
	: type(item_type), key_type(0)
{
}

Item::Item(const Type * full_item_type, const Type * empty_item_type)
	: type(full_item_type), full_type(full_item_type), empty_type(empty_item_type), key_type(0)
{
}

bool Item::valid() const
{
	return type.valid();
}

bool Item::is_emptyable() const
{
	return full_type.valid() && empty_type.valid();
}

bool Item::is_full() const
{
	return type == full_type;
}

bool Item::is_empty() const
{
	return type == empty_type;
}

bool Item::make_full()
{
	if(is_emptyable()) {
		type = full_type;
		return true;
	}
	return false;
}

bool Item::make_empty()
{
	if(is_emptyable()) {
		type = empty_type;
		return true;
	}
	return false;
}


Item::Builder & Item::Builder::pos(const Point & value) { result.pos = value; return *this; }
Item::Builder & Item::Builder::key_type(int value) { result.key_type = value; return *this; }


unsigned Inventory::NOTHING = static_cast<unsigned>(-1);

Inventory::Inventory()
	: wielded(NOTHING), worn(NOTHING)
{
}

bool Inventory::empty() const
{
	foreach(const Item & item, items) {
		if(item.valid()) {
			return false;
		}
	}
	return true;
}

unsigned Inventory::size() const
{
	unsigned result = 0;
	foreach(const Item & item, items) {
		if(item.valid()) {
			++result;
		}
	}
	return result;
}

bool Inventory::set_item(unsigned slot, const Item & item)
{
	if(slot < 0 || SLOT_COUNT <= slot) {
		return false;
	}
	if(slot >= items.size()) {
		items.resize(slot + 1);
	}
	items[slot] = item;
	return true;
}

Item Inventory::take_item(unsigned slot)
{
	if(slot < 0 || items.size() <= slot) {
		return Item();
	}
	if(wielded == slot) {
		unwield();
	}
	if(worn == slot) {
		take_off();
	}
	Item result = items[slot];
	items[slot] = Item();
	return result;
}

Item Inventory::take_first_item()
{
	for(unsigned slot = 0; slot < items.size(); ++slot) {
		if(items[slot].valid()) {
			return take_item(slot);
		}
	}
	return Item();
}

Item Inventory::take_wielded_item()
{
	return take_item(wielded);
}

Item Inventory::take_worn_item()
{
	return take_item(worn);
}

bool Inventory::wield(unsigned slot)
{
	if(slot < 0 || items.size() <= slot) {
		return false;
	}
	wielded = slot;
	return true;
}

bool Inventory::wields(unsigned slot) const
{
	return wielded == slot;
}

void Inventory::unwield()
{
	wielded = NOTHING;
}

bool Inventory::wears(unsigned slot) const
{
	return worn == slot;
}

bool Inventory::wear(unsigned slot)
{
	if(slot < 0 || items.size() <= slot) {
		return false;
	}
	worn = slot;
	return true;
}

void Inventory::take_off()
{
	worn = NOTHING;
}

bool Inventory::has_item(unsigned slot) const
{
	if(slot < 0 || items.size() <= slot) {
		return false;
	}
	return items[slot].valid();
}

const Item & Inventory::get_item(unsigned slot) const
{
	if(slot < 0 || items.size() <= slot) {
		static Item empty;
		return empty;
	}
	return items[slot];
}

Item & Inventory::get_item(unsigned slot)
{
	if(slot < 0 || items.size() <= slot) {
		static Item empty;
		empty = Item();
		return empty;
	}
	return items[slot];
}

void Inventory::clear()
{
	wielded = NOTHING;
	worn = NOTHING;
	items.clear();
}

unsigned Inventory::insert(const Item & item)
{
	if(items.size() >= SLOT_COUNT) {
		return NOTHING;
	}
	for(unsigned slot = 0; slot < items.size(); ++slot) {
		if(!items[slot].valid()) {
			items[slot] = item;
			return slot;
		}
	}
	if(items.size() < SLOT_COUNT) {
		items.resize(items.size() + 1);
	}
	items[items.size() - 1] = item;
	return items.size() - 1;
}

const Item & Inventory::quest_item() const
{
	foreach(const Item & item, items) {
		if(item.type->quest) {
			return item;
		}
	}
	static Item empty;
	return empty;
}

bool Inventory::has_key(int key_type) const
{
	foreach(const Item & item, items) {
		if(item.key_type == key_type) {
			return true;
		}
	}
	return false;
}


}
