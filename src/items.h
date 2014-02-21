#pragma once
#include "map.h"
#include "types.h"
#include "point.h"
#include "util.h"
#include <string>

namespace Chthon { /// @defgroup Items
/// @{

struct ItemType {
	std::string id;
	int sprite;
	std::string name;
	int damage;
	bool wearable;
	int defence;
	bool edible;
	int antidote;
	int healing;
	bool quest;
	ItemType(const std::string & type_id = std::string());

	struct Builder;
};
struct ItemType::Builder {
	ItemType value_result;
	ItemType & result;
	Builder(const std::string & type_id) : value_result(type_id), result(value_result) {}
	Builder(ItemType & type) : result(type) {}
	operator ItemType() { return result; }
	Builder & sprite(const int & value);
	Builder & name(const std::string & value);
	Builder & damage(int damage);
	Builder & wearable();
	Builder & defence(int defence);
	Builder & edible();
	Builder & antidote(int value);
	Builder & healing(int value);
	Builder & quest();
};

struct Item {
	typedef ItemType Type;
	const Type * type;
	const Type * full_type;
	const Type * empty_type;

	Point pos;
	int key_type;
	Item(const Type * item_type = nullptr);
	Item(const Type * full_item_type, const Type * empty_item_type);
	bool valid() const;

	bool is_emptyable() const;
	bool is_full() const;
	bool is_empty() const;
	bool make_full();
	bool make_empty();

	struct Builder;
};
struct Item::Builder {
	Item value_result;
	Item & result;
	Builder(const Type * type) : value_result(type), result(value_result) {}
	Builder(const Type * full_type, const Type * empty_type) : value_result(full_type, empty_type), result(value_result) {}
	Builder(Item & item) : result(item) {}
	operator Item() { return result; }
	Builder & pos(const Point & value);
	Builder & key_type(int value);
	Builder & make_full() { result.make_full(); return *this; }
	Builder & make_empty() { result.make_empty(); return *this; }
};

struct Inventory {
	enum { SLOT_COUNT = 26 };
	static unsigned NOTHING;

	unsigned wielded, worn;
	std::vector<Item> items;

	Inventory();
	bool empty() const;
	void clear();
	unsigned size() const;
	bool set_item(unsigned slot, const Item & item);
	bool has_item(unsigned slot) const;
	const Item & get_item(unsigned slot) const;
	Item & get_item(unsigned slot);

	unsigned insert(const Item & item);
	Item take_item(unsigned slot);
	Item take_first_item();
	Item take_wielded_item();
	Item take_worn_item();

	bool wield(unsigned slot);
	bool wields(unsigned slot) const;
	void unwield();
	const Item & wielded_item() const { return get_item(wielded); }
	Item & wielded_item() { return get_item(wielded); }

	bool wear(unsigned slot);
	bool wears(unsigned slot) const;
	void take_off();
	const Item & worn_item() const { return get_item(worn); }
	Item & worn_item() { return get_item(worn); }

	const Item & quest_item() const;
	bool has_key(int key_type) const;
};

/// @}
}
