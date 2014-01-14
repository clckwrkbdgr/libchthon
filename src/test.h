#pragma once
#include "format.h"
#include <list>
#include <iostream>

namespace Chthon {

namespace UnitTest {

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

std::list<Test*> & all_tests();
int run_all_tests(int argc, char ** argv);

struct AssertException {
	const char * filename;
	int line;
	std::string what;
	AssertException(const char * ex_filename, int ex_linenumber, const std::string & message);
};

const char * current_suite_name();
#define SUITE(suite_name) \
	namespace Suite_##suite_name { \
		const char * current_suite_name() { return #suite_name; } \
	} \
	namespace Suite_##suite_name

#define TEST(test_name) \
	class Test_##test_name : public Test { \
	public: \
		Test_##test_name(const char * suite, const char * name) : Test(suite, name, __FILE__, __LINE__) {} \
		virtual void run(); \
	}; \
	Test_##test_name test_##test_name(current_suite_name(), #test_name); \
	void Test_##test_name::run()

#define TEST_FIXTURE(fixture_name, test_name) \
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

template<class A, class B>
void test_equal(const A & a, const B & b, const char * a_string, const char * b_string, const char * file, int line)
{
	if(a != b) {
		throw AssertException(file, line, std::string(a_string) + " (" + to_string(a) + ") != "  + b_string + " (" + to_string(b) + ")");
	}
}
#define EQUAL(a, b) \
	test_equal(a, b, #a, #b, __FILE__, __LINE__)

template<class ContainerA, class ContainerB>
void test_equal_containers(const ContainerA & a, const ContainerB & b, const char * a_string, const char * b_string, const char * file, int line)
{
	if(!equal_containers(a.begin(), a.end(), b.begin(), b.end())) {
		throw AssertException(file, line, std::string(a_string) + " (" + to_string(a) + ") != "  + b_string + " (" + to_string(b) + ")");
	}
}
#define EQUAL_CONTAINERS(a, b) \
	test_equal_containers(a, b, #a, #b, __FILE__, __LINE__)

#define FAIL(message) \
	throw AssertException(__FILE__, __LINE__, message)

#define ASSERT(expression) \
	do { if(!(expression)) { throw AssertException(__FILE__, __LINE__, "failed assertion: " #expression ); } } while(0)

#define CATCH(expression, exception_class, exception_variable) \
	try { \
		do { (expression); } while(0); \
		throw AssertException(__FILE__, __LINE__, "expected exception " #exception_class " was not thrown"); \
	} catch(const exception_class & exception_variable)

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

#define TEST_CONTAINER(container, var) \
	TestContainer<decltype(container)> test_c_##var(__FILE__, __LINE__, container, #container); \
	for(; !test_c_##var.done(); ) \
	for(decltype(test_c_##var.value()) & var = test_c_##var.value(); !test_c_##var.done(); test_c_##var.mark_done())

#define NEXT(var) \
	for(test_c_##var.to_next(__FILE__, __LINE__); !test_c_##var.done(); ) \
	for(decltype(test_c_##var.value()) & var = test_c_##var.value(); !test_c_##var.done(); test_c_##var.mark_done())

#define DONE(var) \
	test_c_##var.check_at_end(__FILE__, __LINE__)

}

}
