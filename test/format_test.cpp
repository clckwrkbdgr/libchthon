#include "../src/util.h"
#include "../src/format.h"
#include "../src/test.h"

SUITE(format) {

TEST(should_convert_int_to_string)
{
	std::string result = Chthon::to_string(1);
	EQUAL(result, "1");
	result = Chthon::to_string(-1);
	EQUAL(result, "-1");
}

TEST(should_convert_unsigned_to_string)
{
	std::string result = Chthon::to_string(unsigned(1));
	EQUAL(result, "1");
}

TEST(should_convert_unsigned_long_to_string)
{
	std::string result = Chthon::to_string((unsigned long)1);
	EQUAL(result, "1");
}

TEST(should_convert_char_to_string)
{
	std::string result = Chthon::to_string('a');
	EQUAL(result, "a");
}

TEST(should_convert_string_to_string)
{
	std::string result = Chthon::to_string(std::string("hello"));
	EQUAL(result, "hello");
}

TEST(should_format_string_with_one_argument)
{
	std::string result = Chthon::format("hello {0}", "world");
	EQUAL(result, "hello world");
}

TEST(should_format_string_with_more_than_one_argument)
{
	std::string result = Chthon::format("{0} {1}", "hello", "world");
	EQUAL(result, "hello world");
}

TEST(should_not_format_string_with_no_arguments)
{
	std::string result = Chthon::format("hello world", 1, 2);
	EQUAL(result, "hello world");
}

TEST(should_take_args_in_any_order)
{
	std::string result = Chthon::format("{1} {0}", "hello", "world");
	EQUAL(result, "world hello");
}

TEST(should_leave_unused_placeholders_intact)
{
	std::string result = Chthon::format("hello {1}", "hello");
	EQUAL(result, "hello {1}");
}

}
