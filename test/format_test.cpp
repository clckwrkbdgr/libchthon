#include "../src/util.h"
#include "../src/format.h"
#include "../src/test.h"
#include <sstream>

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

TEST(should_fill_with_spaces_by_default_when_padding_in_format)
{
	std::string result = Chthon::format("<{0:5}>", 1);
	EQUAL(result, "<    1>");
}

TEST(should_fill_with_zeroes_when_specified_in_format)
{
	std::string result = Chthon::format("<{0:05}>", 1);
	EQUAL(result, "<00001>");
}

TEST(should_convert_int_to_hex_in_format)
{
	std::string result = Chthon::format("<{0:#}>", 255);
	EQUAL(result, "<ff>");
}

TEST(should_convert_unsigned_to_hex_in_format)
{
	std::string result = Chthon::format("<{0:#}>", 255u);
	EQUAL(result, "<ff>");
}

TEST(should_convert_unsigned_char_to_hex_in_format)
{
	uint8_t i = 255;
	std::string result = Chthon::format("<{0:#}>", i);
	EQUAL(result, "<ff>");
}

}
