#include "test.h"
#include "log.h"
#include "util.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <signal.h>

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

const char * current_suite_name()
{
	return "";
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
		try {
			test->run();
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
			std::cout << "[ OK ] " << test_name << std::endl;
			++passed_tests;
		} else {
			std::cout << "[FAIL] " << test_name << std::endl;
			std::cerr << exception_text << std::endl;
		}
	}
	int failed_tests = total_test_count - passed_tests;
	if(total_test_count == 0) {
		std::cout << "No tests to run." << std::endl;
	} else if(failed_tests == 0) {
		std::cout << "All tests are passed." << std::endl;
	} else {
		std::cout << failed_tests << ((failed_tests % 10 == 1) ? " test" : " tests") << " failed!" << std::endl;
	}
	/// @return count of failed tests.
	return failed_tests;
}

}
