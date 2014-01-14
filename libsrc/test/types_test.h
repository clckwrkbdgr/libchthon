#include "../types.h"
#include "../test.h"

SUITE(types) {

struct Value {
	struct Type {
		std::string id;
		int sprite;
		Type() : sprite(0) {}
		Type(const std::string & type_id, int type_sprite) : id(type_id), sprite(type_sprite) {}
		struct Builder;
	};
	const Type * type;
	Value(const Type * value_type = 0) : type(value_type) {}
};
struct Value::Type::Builder {
	Value::Type value_result;
	Value::Type & result;
	Builder(const std::string & type_id) : value_result(type_id, 0), result(value_result) {}
	Builder(Value::Type & type) : result(type) {}
	operator Value::Type() { return result; }
	Builder & sprite(const int & value) { result.sprite = value; return *this; }
};

TEST(should_assume_non_null_type_pointer_as_valid)
{
	Value::Type type("type", 1);
	TypePtr<Value::Type> ptr(&type);
	ASSERT(ptr.valid());
}

TEST(should_assume_null_type_pointer_as_invalid)
{
	TypePtr<Value::Type> ptr;
	ASSERT(!ptr.valid());
}

TEST(should_return_type_from_pointer_if_valid)
{
	Value::Type type("type", 1);
	TypePtr<Value::Type> ptr(&type);
	EQUAL(ptr->id, "type");
}

TEST(should_return_default_type_from_pointer_if_invalid)
{
	TypePtr<Value::Type> ptr;
	ASSERT(ptr->id.empty());
}

TEST(should_get_type_for_value)
{
	TypeRegistry<Value> reg;
	reg.insert(Value::Type("test", 1));
	ASSERT(reg.get("test"));
}

TEST(should_return_default_empty_type_for_unknown_cell_type)
{
	TypeRegistry<Value> reg;
	reg.insert(Value::Type("known", 1));
	ASSERT(!reg.get("unknown"));
}

TEST(should_return_builder_when_inserting)
{
	TypeRegistry<Value> reg;
	reg.insert("test").sprite(1);
	ASSERT(reg.get("test"));
	EQUAL(reg.get("test")->sprite, 1);
}

}
