#pragma once
#include "format.h"
#include <list>
#include <iostream>

namespace Chthon { /// @defgroup UnitTest Unit testing
/// @{

/// Runs all tests.
int run_all_tests(int argc, char ** argv);

/// @cond INTERNAL
struct Test {
	const char * suite;
	const char * name;
	const char * filename;
	int line;
	Test(const char * test_suite, const char * test_name, const char * filename, int line);
	virtual ~Test() {}
	virtual void run() = 0;
	bool specified(int argc, char ** argv) const;
};

struct AssertException {
	const char * filename;
	int line;
	std::string what;
	AssertException(const char * ex_filename, int ex_linenumber, const std::string & message);
};
/// @endcond

/// @cond INTERNAL
const char * current_suite_name();
/// @endcond

/** Defines test suite.
 * Test suite is a collection of tests gathere under mutual name.
 * Works pretty much as a namespace except that suites cannot be nested.
 *
 * Example usage:
 * @code{.cpp}
 * SUITE(test_suite) {
 *     TEST(should_do_something) { ... }
 * }
 * @endcode
 */
#define SUITE(suite_name) \
	namespace Suite_##suite_name { \
		static const char * current_suite_name() { return #suite_name; } \
	} \
	namespace Suite_##suite_name

/** Defines test case.
 * First failed assertion breaks test execution. Failed file name and line number if printed to the stdout along with error message.
 * If any uncatched exception is thrown, it will be displayed on stdout.
 * Also segfault is catched, but the whole flow is interrupted in that case.
 */
#define TEST(test_name) \
	class Test_##test_name : public Chthon::Test { \
	public: \
		Test_##test_name(const char * test_suite, const char * test_name) : Test(test_suite, test_name, __FILE__, __LINE__) {} \
		virtual void run(); \
	}; \
	Test_##test_name test_##test_name(current_suite_name(), #test_name); \
	void Test_##test_name::run()

/** Defines test case that uses fixture.
 * Fixture must be C++ struct or class with public or protected properies and methods and public default ctor/dtor.
 * Test case sets up fixture on start (using its default ctor) and all public and protected member are available as in-scope variables.
 *
 * @code{.cpp}
 * struct Fixture {
 *     int i;
 *     Fixture() : i(10) {}
 * };
 *
 * TEST_FIXTURE(Fixture, should_do_something)
 * {
 *     EQUAL(i, 10);
 * }
 * @endcode
 */
#define TEST_FIXTURE(fixture_name, test_name) \
	inline void test_class_existence_##test_name(const fixture_name &) {} \
	class Fixture_##fixture_name##test_name : public fixture_name { \
	public: \
		void run(); \
	}; \
	TEST(test_name) \
	{ \
		Fixture_##fixture_name##test_name fixture; \
		fixture.run(); \
	} \
	void Fixture_##fixture_name##test_name::run()

/// @cond INTERNAL
template<class A, class B>
void test_equal(const A & a, const B & b, const char * a_string, const char * b_string, const char * file, int line)
{
	if(a != b) {
		throw AssertException(file, line, Chthon::format("{0} ({1}) != {2} ({3})", a_string, a, b_string, b));
	}
}
/// @endcond

/** Checks that a equals b.
 * Tested objects must support `operator !=` and have functions `to_string` defined for each one.
 * Failed assertion breaks test execution.
 * This is a test assert and should be using in TEST() case only, as it throws specific Exception.
 */
#define EQUAL(a, b) \
	Chthon::test_equal(a, b, #a, #b, __FILE__, __LINE__)

/** Uncoditionally fails with specified error message and breaks test execution.
 * This is a test assert and should be using in TEST() case only, as it throws specific Exception.
 */
#define FAIL(message) \
	throw Chthon::AssertException(__FILE__, __LINE__, message)

/** Simple assert check. Treats expression as a boolean and checks its value after evaluation.
 * If value is false, it fails and breaks test execution.
 * This is a test assert and should be using in TEST() case only, as it throws specific Exception.
 */
#define ASSERT(expression) \
	do { if(!(expression)) { throw Chthon::AssertException(__FILE__, __LINE__, "failed assertion: " #expression ); } } while(0)

/** Checks whether exception throws specified exception of exception_class.
 * When exception is thrown, it is accesible as exception_variable.
 * This is a test assert and should be using in TEST() case only, as it throws specific Exception.
 * If no exception is thrown, test is interrupted with error message.
 * If any other exception is thrown, it will interrupt test case as usual.
 *
 * @code{.cpp}
 * CATCH(throw std::logic_error("Its a trap!"), std::logic_error, e) {
 *     EQUAL(e.what(), "Its a trap!");
 * }
 * @endcode
 */
#define CATCH(expression, exception_class, exception_variable) \
	try { \
		do { (expression); } while(0); \
		throw Chthon::AssertException(__FILE__, __LINE__, "expected exception " #exception_class " was not thrown"); \
	} catch(const exception_class & exception_variable)

/// @cond INTERNAL
template<class Container>
struct TestContainer {
	TestContainer(const char * filename, int line, const Container & test_container, const std::string & container_name)
		: is_done(false), container(test_container), name(container_name)
	{
		it = container.begin();
		if(it == container.end()) {
			throw AssertException(filename, line, "Expected value, but the end of " + name + " was found.");
		}
	}
	bool done() const { return is_done; }
	void to_next(const char * filename, int line)
	{
		if(++it == container.end()) {
			throw AssertException(filename, line, "Expected value, but the end of " + name + " was found.");
		}
		is_done = false;
	}
	void mark_done() { is_done = true; }
	const typename Container::value_type & value() const { return *it; }
	void check_at_end(const char * filename, int line)
	{
		if(++it != container.end()) {
			throw AssertException(filename, line, "Expected end of " + name + ", but value was found.");
		}
	}
private:
	bool is_done;
	const Container & container;
	const std::string name;
	typename Container::const_iterator it;
};
/// @endcond

/** Tests container values.
 * It will iterate through the container.
 * Current item is accessible as a loop variable var.
 * Loop variable name should be unique within test case.
 * First item is ready immediately after TEST_CONTAINER call.
 * If there is no single item, assert will fail.
 * Iterating is done using NEXT(var).
 * Optional assert DONE(var) checks whether there is no more values left in container.
 * @see NEXT()
 * @see DONE()
 *
 * @code{.cpp}
 * std::vector<int> v{1, 2, 3};
 * TEST_CONTAINER(v, i) {
 *     EQUAL(i, 1);
 * } NEXT(i) {
 *     EQUAL(i, 2);
 * } NEXT(i) {
 *     EQUAL(i, 3);
 * } DONE(i);
 * @endcode
 */
#define TEST_CONTAINER(container, var) \
	Chthon::TestContainer<decltype(container)> test_c_##var(__FILE__, __LINE__, container, #container); \
	for(; !test_c_##var.done(); ) \
	for(decltype(test_c_##var.value()) & var = test_c_##var.value(); !test_c_##var.done(); test_c_##var.mark_done())

/** Iterates through container in TEST_CONTAINER assert.
 * Current value is ready after the call.
 * If there is no item to pick next, assert will fail.
 * @see TEST_CONTAINER()
 * @see DONE()
 */
#define NEXT(var) \
	for(test_c_##var.to_next(__FILE__, __LINE__); !test_c_##var.done(); ) \
	for(decltype(test_c_##var.value()) & var = test_c_##var.value(); !test_c_##var.done(); test_c_##var.mark_done())

/** Checks that container is iterated to the end.
 * If there are items left, assert will fail.
 * @see TEST_CONTAINER()
 * @see NEXT()
 */
#define DONE(var) \
	test_c_##var.check_at_end(__FILE__, __LINE__)

/// @}
}
