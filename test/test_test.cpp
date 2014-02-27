#include "../src/test.h"
#include <vector>
#include <stdexcept>

TEST(test_without_suite)
{
	EQUAL(_suite, "");
}

SUITE(test_suite) {

TEST(should_know_test_suite_name)
{
	EQUAL(_suite, "test_suite");
}

struct Fixture {
	int i;
	Fixture() : i(1) {}
};

TEST_FIXTURE(Fixture, should_test_fixture)
{
	EQUAL(i, 1);
}

TEST(should_fail_when_not_equal)
{
	bool thrown = false;
	try {
		EQUAL(1, 2);
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(thrown);
}

TEST(should_pass_when_equal)
{
	bool thrown = false;
	try {
		EQUAL(1, 1);
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(!thrown);
}

TEST(should_always_fail_when_fail)
{
	bool thrown = false;
	try {
		FAIL("test");
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(thrown);
}

TEST(should_fail_when_assert_is_false)
{
	bool thrown = false;
	try {
		ASSERT(false);
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	if(!thrown) {
		throw Chthon::AssertException(__FILE__, __LINE__, "Assert isn't working!");
	}
}

TEST(should_pass_when_assert_is_true)
{
	bool thrown = false;
	try {
		ASSERT(true);
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	if(thrown) {
		throw Chthon::AssertException(__FILE__, __LINE__, "Assert isn't working!");
	}
}

TEST(should_catch_exceptions)
{
	bool thrown = false;
	try {
		CATCH(throw std::logic_error("Its a trap!"), const std::logic_error & e) {
			EQUAL(e.what(), "Its a trap!");
		}
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(!thrown);
}

TEST(should_fail_when_expected_exception_isnt_raised)
{
	bool thrown = false;
	try {
		int i = 1;
		CATCH(i = 2, const std::logic_error & e) {
			EQUAL(e.what(), "Its a trap!");
		}
		EQUAL(i, 1);
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(thrown);
}

TEST(should_fail_when_exception_is_thrown_in_nothrow)
{
	bool thrown = false;
	try {
		NOTHROW(throw std::logic_error("Its a trap!"));
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(thrown);
}

TEST(should_pass_when_no_exception_is_thrown)
{
	bool thrown = false;
	try {
		int i = 1;
		NOTHROW(i = 2);
		EQUAL(i, 2);
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(!thrown);
}

TEST(should_test_container)
{
	std::vector<int> v = {1, 2, 3};
	TEST_CONTAINER(v, i) {
		EQUAL(i, 1);
	} NEXT(i) {
		EQUAL(i, 2);
	} NEXT(i) {
		EQUAL(i, 3);
	} DONE(i);
}

TEST(should_fail_if_container_is_empty)
{
	bool thrown = false;
	try {
		std::vector<int> v;
		TEST_CONTAINER(v, i) {
			EQUAL(i, 1);
		}
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(thrown);
}

TEST(should_fail_if_value_is_missing)
{
	bool thrown = false;
	try {
		std::vector<int> v = {1};
		TEST_CONTAINER(v, i) {
			EQUAL(i, 1);
		} NEXT(i) {
			EQUAL(i, 2);
		}
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(thrown);
}

TEST(should_fail_if_items_left_when_done)
{
	bool thrown = false;
	try {
		std::vector<int> v = {1, 2, 3};
		TEST_CONTAINER(v, i) {
			EQUAL(i, 1);
		} DONE(i);
	} catch(const Chthon::AssertException & e) {
		thrown = true;
	}
	ASSERT(thrown);
}

TEST_DATA(test_data, 1, 1, should_test_data);
TEST_DATA(test_data, 2, 2, should_test_data_again)
{
	EQUAL(test_data_data, test_data_expected);
}

}
