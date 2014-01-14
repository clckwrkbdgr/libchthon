#include "../src/objects.h"
#include "../src/game.h"
#include "../src/test.h"

using namespace Chthon::UnitTest;

namespace Chthon {

SUITE(objects) {

TEST(should_not_open_not_openable_object)
{
	ObjectType type = ObjectType::Builder("test");
	Object object = Object::Builder(&type);
	bool ok = object.open();
	ASSERT(!ok);
}

TEST(should_not_close_not_openable_object)
{
	ObjectType type = ObjectType::Builder("test");
	Object object = Object::Builder(&type);
	bool ok = object.close();
	ASSERT(!ok);
}

TEST(object_should_be_of_opened_type_after_opening)
{
	ObjectType opened_door = ObjectType::Builder("opened").openable().sprite(1);
	ObjectType closed_door = ObjectType::Builder("closed").openable().sprite(2);
	Object object = Object::Builder(&closed_door, &opened_door).opened(false);
	bool ok = object.open();
	ASSERT(ok);
	EQUAL(object.type->id, "opened");
}

TEST(object_should_be_of_closed_type_after_closing)
{
	ObjectType opened_door = ObjectType::Builder("opened").openable().sprite(1);
	ObjectType closed_door = ObjectType::Builder("closed").openable().sprite(2);
	Object object = Object::Builder(&closed_door, &opened_door).opened(true);
	bool ok = object.close();
	ASSERT(ok);
	EQUAL(object.type->id, "closed");
}


TEST(negative_up_destination_should_be_exit_from_dungeon)
{
	ObjectType type = ObjectType::Builder("test").transporting();
	Object object = Object::Builder(&type).up_destination(-1);
	ASSERT(object.is_exit_up());
}

TEST(negative_down_destination_should_be_exit_from_dungeon)
{
	ObjectType type = ObjectType::Builder("test").transporting();
	Object object = Object::Builder(&type).down_destination(-1);
	ASSERT(object.is_exit_down());
}

}

}
