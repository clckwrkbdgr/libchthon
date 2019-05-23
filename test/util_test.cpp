#include "../src/util.h"
#include "../src/test.h"

SUITE(util) {

TEST(bound_should_return_min_when_value_is_lesser_than_min)
{
	EQUAL(Chthon::bound(0, -1, 10), 0);
}

TEST(bound_should_return_max_when_value_is_greater_than_max)
{
	EQUAL(Chthon::bound(0, 11, 10), 10);
}

TEST(bound_should_return_value_when_value_is_inside_the_interval)
{
	EQUAL(Chthon::bound(0, 5, 10), 5);
}

TEST(should_return_sign_of_positive_number)
{
	int result = Chthon::sign(10);
	EQUAL(result, 1);
}

TEST(should_return_sign_of_negative_number)
{
	int result = Chthon::sign(-10);
	EQUAL(result, -1);
}

TEST(should_return_sign_of_zero)
{
	int result = Chthon::sign(0);
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
			default: FAIL("Unknown switch case");
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

TEST(should_split_string)
{
	std::vector<std::string> tokens = Chthon::split("foo\nbar\nbaz\n");
	TEST_CONTAINER(tokens, token) {
		EQUAL(token, "foo");
	} NEXT(token) {
		EQUAL(token, "bar");
	} NEXT(token) {
		EQUAL(token, "baz");
	} DONE(token);
}

TEST(should_return_true_if_string_starts_with_pattern)
{
	ASSERT(Chthon::starts_with("hello world", "hell"));
}

TEST(should_return_false_if_string_does_not_starts_with_pattern)
{
	ASSERT(!Chthon::starts_with("hello world", "heaven"));
}

TEST(should_return_true_if_string_ends_with_pattern)
{
	ASSERT(Chthon::ends_with("foo bar baz", "az"));
}

TEST(should_return_false_if_string_does_not_ends_with_pattern)
{
	ASSERT(!Chthon::ends_with("foo bar baz", "foo"));
}

TEST(should_collapse_extra_whitespaces)
{
	EQUAL(Chthon::collapse_whitespaces("  Text\nwith    whitespaces\t"),
			" Text with whitespaces ");
}

TEST(should_collapse_specified_whitespaces)
{
	EQUAL(Chthon::collapse_whitespaces("  Text\nwith    whitespaces\t", " "),
			" Text\nwith whitespaces\t");
}

TEST(should_trim_heading_whitespaces)
{
	EQUAL(Chthon::trim_left("   \nText\nwith    whitespaces\t"),
			"Text\nwith    whitespaces\t");
}

TEST(should_trim_specified_heading_whitespaces)
{
	EQUAL(Chthon::trim_left("   \nText\nwith    whitespaces\t", " "),
			"\nText\nwith    whitespaces\t");
}

TEST(should_trim_trailing_whitespaces)
{
	EQUAL(Chthon::trim_right("  Text\nwith    whitespaces\t  "),
			"  Text\nwith    whitespaces");
}

TEST(should_trim_specified_trailing_whitespaces)
{
	EQUAL(Chthon::trim_right("  Text\nwith    whitespaces\t  ", " "),
			"  Text\nwith    whitespaces\t");
}

TEST(should_trim_extra_whitespaces)
{
	std::string strings[] = {
		"  Text\nwith    whitespaces\t",
		"  Text\nwith    whitespaces",
		"Text\nwith    whitespaces\t",
		"Some text **with bold _and italic_**",
	};

	EQUAL(Chthon::trim(strings[0]),
			Chthon::trim_right(Chthon::trim_left(strings[0])));
	EQUAL(Chthon::trim(strings[1]),
			Chthon::trim_right(Chthon::trim_left(strings[1])));
	EQUAL(Chthon::trim(strings[2]),
			Chthon::trim_right(Chthon::trim_left(strings[2])));
	EQUAL(Chthon::trim(strings[3]),
			Chthon::trim_right(Chthon::trim_left(strings[3])));
}

TEST(should_trim_only_specified_whitespaces)
{
	std::string s = "  Text\nwith    whitespaces\t";
	EQUAL(Chthon::trim(s, "\t"),
			Chthon::trim_right(Chthon::trim_left(s, "\t"), "\t"));
	EQUAL(Chthon::trim(s, " "),
			Chthon::trim_right(Chthon::trim_left(s, " "), " "));
}

TEST(should_know_if_string_contains_substring)
{
	std::string s = "hello world";
	std::string h = "hell";
	std::string w = "war";
	ASSERT(Chthon::contains(s, h));
	ASSERT(!Chthon::contains(s, w));
}

TEST(should_know_if_string_contains_character)
{
	std::string s = "hello world";
	ASSERT(Chthon::contains(s, 'h'));
	ASSERT(!Chthon::contains(s, 'a'));
}

TEST(should_know_if_c_string_contains_substring)
{
	ASSERT(Chthon::contains("hello world", "hell"));
	ASSERT(!Chthon::contains("hello world", "war"));
}

TEST(should_know_if_c_string_contains_character)
{
	ASSERT(Chthon::contains("hello world", 'h'));
	ASSERT(!Chthon::contains("hello world", 'a'));
}

TEST(should_know_if_vector_contains_value)
{
	std::vector<int> v = {1, 2, 3};
	ASSERT(Chthon::contains(v, 1));
	ASSERT(!Chthon::contains(v, 0));
}

TEST(should_know_if_list_contains_value)
{
	std::list<int> v = {1, 2, 3};
	ASSERT(Chthon::contains(v, 1));
	ASSERT(!Chthon::contains(v, 0));
}

TEST(should_know_if_map_contains_key)
{
	std::map<int, char> m = {{1, 'a'}, {2, 'b'}, {3, 'c'}};
	ASSERT(Chthon::contains(m, 1));
	ASSERT(!Chthon::contains(m, 0));
}

TEST(should_add_to_vector_and_return_index)
{
	std::vector<int> v = {1, 2, 3};
	size_t index = Chthon::add_to(v, 10);
	EQUAL(index, 3ul);
}

TEST(should_extract_interleaved_charmap)
{
	Chthon::InterleavedCharMap map = { 3, 3, 4, {
		"1  ","222","abc","4 4",
		" 1 ","2 2","def","444",
		"  1","222","ghi","  4",
	}};

	std::string result(map.begin(2), map.end(2));
	EQUAL(result, "abcdefghi");
}

}
