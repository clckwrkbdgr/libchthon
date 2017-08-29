#include "test.h"
#include "log.h"
#include "util.h"
#include <chrono>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <signal.h>

namespace Colors {
	const char * DEFAULT = "\033[39m";
	const char * RED = "\033[31m";
	const char * GREEN = "\033[32m";
	const char * BLUE = "\033[34m";
}

const char * current_suite_name()
{
	return "";
}

namespace Chthon {

/// @cond INTERNAL
static std::list<Test*> & all_tests()
{
	static std::list<Test*> tests;
	return tests;
}

Test::Test(const char * test_suite, const char * test_name, const char * test_filename, int test_line)
	: _suite(test_suite), _name(test_name), _filename(test_filename), _line(test_line)
{
	all_tests().push_back(this);
}

bool Test::specified(int argc, char ** argv) const
{
	for(int i = 1; i < argc; ++i) {
		if(strcmp(_name, argv[i]) == 0 || strcmp(_suite, argv[i]) == 0) {
			return true;
		}
	}
	return false;
}

AssertException::AssertException(const char * ex_filename, int ex_linenumber, const std::string & message)
	: filename(ex_filename), line(ex_linenumber), what(message)
{
}

bool are_not_equal(const char * a, const char * b)
{
	return strcmp(a, b) != 0;
}

bool are_not_equal(unsigned int a, int b)
{
	if(b < 0) {
		return true;
	} else {
		return a != unsigned(b);
	}
}

bool are_not_equal(int a, unsigned int b)
{
	return are_not_equal(b, a);
}

static const char * current_filename = __FILE__;
static int current_line = __LINE__;

static void catch_segfault(int)
{
	printf("%s:%d: segmentation fault\n", current_filename, current_line);
	exit(1);
}

/// @endcond

int run_all_tests(int argc, char ** argv)
{
	signal(SIGSEGV, catch_segfault);

	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout.precision(3);
	bool tests_specified = argc > 1;
	int total_test_count = 0;
	int passed_tests = 0;
	for(std::list<Test*>::iterator it = all_tests().begin(); it != all_tests().end(); ++it) {
		Test * test = *it;
		/** If command-line parameters are being supplied, they're treated as test or suite names to run.
		 * In that case only the specified test cases or test suites are executed.
		 */
		if(tests_specified && !test->specified(argc, argv)) {
			continue;
		}
		current_filename = test->_filename;
		current_line = test->_line;
		++total_test_count;
		bool ok = true;
		std::string exception_text;
		std::string test_name = test->_suite;
		/// Test name is printed with its suite name, e.g. "test_suite :: test_name".
		/// If test is outside any suite, it has no suite name.
		test_name += std::string(test_name.empty() ? "" : " :: ") + test->_name;
		/// Each test is also a benchmark using steady clock.
		std::chrono::nanoseconds duration = std::chrono::nanoseconds::zero();
		try {
			const auto start_time = std::chrono::steady_clock::now();
			test->run();
			const auto end_time = std::chrono::steady_clock::now();
			duration = end_time - start_time;
		} catch(const AssertException & e) {
			ok = false;
			exception_text = std::string(e.filename) + ":" + std::to_string(e.line) + ": " + e.what;
		} catch(const std::exception & e) {
			ok = false;
			exception_text = std::string(test->_filename) + ":" + std::to_string(test->_line) + ": exception caught: " + e.what();
		} catch(...) {
			ok = false;
			exception_text = std::string(test->_filename) + ":" + std::to_string(test->_line) + ": unknown exception";
		}
		/// Each test result is printed either with `[ OK ]` sign or with '[FAIL]' sign.
		/// Each test failure message is prepended with file name and line number.
		if(ok) {
			std::chrono::duration<double> seconds =
				std::chrono::duration_cast<std::chrono::duration<double>>(duration);
			std::cout << Colors::GREEN << "[ OK ] " << Colors::DEFAULT << test_name << ' '
				<< Colors::BLUE << duration.count() << "ns";
			if(seconds.count() >= 1e-3) {
				std:: cout << " ("
				<< seconds.count() << "s)";
			}
			std::cout << Colors::DEFAULT << std::endl;
			++passed_tests;
		} else {
			std::cout << Colors::RED << "[FAIL] " << Colors::DEFAULT << test_name << std::endl;
			std::cerr << exception_text << std::endl;
		}
	}
	int failed_tests = total_test_count - passed_tests;
	if(total_test_count == 0) {
		std::cout << Colors::RED << "No tests to run." << Colors::DEFAULT << std::endl;
	} else if(failed_tests == 0) {
		std::cout << Colors::GREEN << "All tests are passed." << Colors::DEFAULT << std::endl;
	} else {
		std::cout << Colors::RED << failed_tests <<
			((failed_tests % 10 == 1) ? " test" : " tests") << " failed!"
			<< Colors::DEFAULT << std::endl;
	}
	/// @return count of failed tests.
	return failed_tests;
}

}
