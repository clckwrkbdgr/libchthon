#pragma once
#include "game.h"
#include "info.h"
#include "point.h"

namespace Chthon { /// @defgroup Actions Actions
/// @{

class Game;
class Monster;

/** Basic action class. Represents game action.
 * New action is created by subclassing Action and overriding its pure virtual function commit().
 *
 * Action should be constructed in order to commit:
 * @code{.cpp}
 * Grab action;
 * action.commit(player, game);
 * @endcode
 *
 * GameEvent class serves as the Exception (used by assert()).
 * @see GameEvent
 */
class Action {
public:
	typedef GameEvent Exception;

	virtual ~Action() {}
	/** Commit action.
	 * @param someone monster, which is commiting action.
	 * @param game reference to the game.
	 */
	virtual void commit(Monster & someone, Game & game) = 0;
	/** Evaluates and asserts game related expressions.
	 * If expression evaluates to false, throws corresponding Exception of exception_type and subject and object parameters.
	 * @see GameEvent
	 * @see GameEvent::EventType
	 */
	void assert(bool expression, GameEvent::EventType exception_type, const Info & subject, const Info & object = Info());
};

/** Directed action - subclass of basic Action.
 * It is supplied with direction (or shift) in form of Point.
 */
class DirectedAction : public Action {
public:
	const Point shift;

	/// Constructs directed action using given direction.
	DirectedAction(const Point & action_direction) : shift(action_direction) {}
	virtual ~DirectedAction() {}
};

/** Slot action - subclass of basic actions.
 * It represents all actions with monster's inventory.
 * It is supplied with inventory slot.
 */
class SlotAction : public Action {
public:
	unsigned slot;

	/// Constructs action using given inventory slot.
	SlotAction(unsigned action_slot) : slot(action_slot) {}
	virtual ~SlotAction() {}
};


/** Do nothing.
 */
class Wait : public Action {
public:
	virtual void commit(Monster &, Game &) {}
};

/** Bump into monsters, impassable objects or cells.
 * Otherwise move freely.
 */
class Move : public DirectedAction {
public:
	Move(const Point & action_shift) : DirectedAction(action_shift) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Open closed doors and take items from container.
 * Unlocks locked doors if corresponding key is present.
 */
class Open : public DirectedAction {
public:
	Open(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Close opened doors.
 */
class Close : public DirectedAction {
public:
	Close(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Swing in specified direction.
 * Hit monster, object or cell, if present.
 * Also swinging opens closed (but not locked) doors.
 */
class Swing : public DirectedAction {
public:
	Swing(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Throw wielded object in specified direction.
 * Hit monsters, object and opaque cells.
 * If object is container or fountain, item falls into it.
 * For fountains this means that item is forever lost.
 */
class Fire : public DirectedAction {
public:
	Fire(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Drink (and heal, if needed) from drinkable object.
 */
class Drink : public DirectedAction {
public:
	Drink(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Grab item from the floor.
 */
class Grab : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

/** Drop item on the floor.
 * If it is wielded or worn, it will be taken off or unwielded first.
 */
class Drop : public SlotAction {
public:
	Drop(unsigned action_slot) : SlotAction(action_slot) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Wield item. If it was worn, it will be taken off first.
 */
class Wield : public SlotAction {
public:
	Wield(unsigned action_slot) : SlotAction(action_slot) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Unwield wielded item.
 */
class Unwield : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

/** Wear wearable item. If it was wielded, it will be unwielded first.
 */
class Wear : public SlotAction {
public:
	Wear(unsigned action_slot) : SlotAction(action_slot) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Take off worn item.
 */
class TakeOff : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

/** Eat edible item.
 * If its worn, it will be taken off first.
 * If its wielded, it will be unwielded first (except for refillable items).
 * Item may heal monster or cure poison.
 * If item can be refilled, it is turns empty, otherwise it is lost.
 */
class Eat : public SlotAction {
public:
	Eat(unsigned action_slot) : SlotAction(action_slot) {}
	virtual void commit(Monster & someone, Game & game);
};

/** Go up using stairs.
 * If it is exit from the dungeon, one must carry a quest item in order to exit.
 * Otherwise he will be reminded of that.
 */
class GoUp : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

/** Go down using stairs.
 * If it is exit from the dungeon, one must carry a quest item in order to exit.
 * Otherwise he will be reminded of that.
 */
class GoDown : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

/** Put item into object or on the floor.
 * If object is drinkable and item can be refilled, it is refilled.
 * Otherwise it is dropped on the floor.
 */
class Put : public DirectedAction {
public:
	Put(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

/// @}
}
