#include "../util.h"
#include "../test.h"

using namespace Chthon::UnitTest;

namespace Chthon {

SUITE(util) {

TEST(should_return_sign_of_positive_number)
{
	int result = sign(10);
	EQUAL(result, 1);
}

TEST(should_return_sign_of_negative_number)
{
	int result = sign(-10);
	EQUAL(result, -1);
}

TEST(should_return_sign_of_zero)
{
	int result = sign(0);
	EQUAL(result, 0);
}

TEST(should_iterate_for_each_value_in_vector)
{
	std::vector<int> v; v << 1 << 2 << 3;
	int j = 1;
	foreach(int i, v) {
		EQUAL(i, j);
		++j;
	}
}

TEST(should_iterate_for_each_value_in_vector_by_ref)
{
	std::vector<int> v; v << 1 << 2 << 3;
	foreach(int & i, v) {
		i *= i;
	}
	TEST_CONTAINER(v, i) {
		EQUAL(i, 1);
	} NEXT(i) {
		EQUAL(i, 4);
	} NEXT(i) {
		EQUAL(i, 9);
	} DONE(i);
}

TEST(should_iterate_for_each_char_in_string)
{
	std::string s = "run";
	int j = 0;
	foreach(char ch, s) {
		switch(j) {
			case 0: EQUAL(ch, 'r'); break;
			case 1: EQUAL(ch, 'u'); break;
			case 2: EQUAL(ch, 'n'); break;
		}
		++j;
	}
}

TEST(should_iterate_for_each_char_in_string_by_ref)
{
	std::string s = "abc";
	foreach(char & ch, s) {
		++ch;
	}
	EQUAL(s, "bcd");
}

TEST(should_push_back_value_to_vector)
{
	std::vector<int> v;
	v << 99;
	EQUAL(v.size(), (unsigned)1);
	EQUAL(v[0], 99);
}

TEST(should_push_back_c_string_to_vector)
{
	std::vector<std::string> stringlist;
	stringlist << "hello";
	EQUAL(stringlist.size(), (unsigned)1);
	EQUAL(stringlist[0], "hello");
}

}

}
