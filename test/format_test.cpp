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

}
