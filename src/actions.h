#pragma once
#include "game.h"
#include "info.h"
#include "point.h"

namespace Chthon {

class Game;
class Monster;

class Action {
public:
	typedef GameEvent Exception;

	virtual ~Action() {}
	virtual void commit(Monster & someone, Game & game) = 0;
	void assert(bool expression, GameEvent::EventType exception_type, const Info & subject, const Info & object = Info());
};

class DirectedAction : public Action {
public:
	const Point shift;

	DirectedAction(const Point & action_direction) : shift(action_direction) {}
	virtual ~DirectedAction() {}
};

class SlotAction : public Action {
public:
	unsigned slot;

	SlotAction(unsigned action_slot) : slot(action_slot) {}
	virtual ~SlotAction() {}
};


class Wait : public Action {
public:
	virtual void commit(Monster &, Game &) {}
};

class Move : public DirectedAction {
public:
	Move(const Point & action_shift) : DirectedAction(action_shift) {}
	virtual void commit(Monster & someone, Game & game);
};

class Open : public DirectedAction {
public:
	Open(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

class Close : public DirectedAction {
public:
	Close(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

class Swing : public DirectedAction {
public:
	Swing(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

class Fire : public DirectedAction {
public:
	Fire(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

class Drink : public DirectedAction {
public:
	Drink(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

class Grab : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class Drop : public SlotAction {
public:
	Drop(unsigned action_slot) : SlotAction(action_slot) {}
	virtual void commit(Monster & someone, Game & game);
};

class Wield : public SlotAction {
public:
	Wield(unsigned action_slot) : SlotAction(action_slot) {}
	virtual void commit(Monster & someone, Game & game);
};

class Unwield : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class Wear : public SlotAction {
public:
	Wear(unsigned action_slot) : SlotAction(action_slot) {}
	virtual void commit(Monster & someone, Game & game);
};

class TakeOff : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class Eat : public SlotAction {
public:
	Eat(unsigned action_slot) : SlotAction(action_slot) {}
	virtual void commit(Monster & someone, Game & game);
};

class GoUp : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class GoDown : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class Put : public DirectedAction {
public:
	Put(const Point & direction) : DirectedAction(direction) {}
	virtual void commit(Monster & someone, Game & game);
};

}
