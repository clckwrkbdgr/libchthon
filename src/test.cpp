#include "test.h"
#include "log.h"
#include "util.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <signal.h>

namespace Chthon {

namespace UnitTest {

std::list<Test*> & all_tests()
{
	static std::list<Test*> tests;
	return tests;
}

Test::Test(const char * test_suite, const char * test_name, const char * test_filename, int test_line)
	: suite(test_suite), name(test_name), filename(test_filename), line(test_line)
{
	all_tests().push_back(this);
}

bool Test::specified(int argc, char ** argv) const
{
	for(int i = 1; i < argc; ++i) {
		if(strcmp(name, argv[i]) == 0 || strcmp(suite, argv[i]) == 0) {
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

int run_all_tests(int argc, char ** argv)
{
	signal(SIGSEGV, catch_segfault);

	bool tests_specified = argc > 1;
	int total_test_count = 0;
	int passed_tests = 0;
	for(std::list<Test*>::iterator it = all_tests().begin(); it != all_tests().end(); ++it) {
		Test * test = *it;
		if(tests_specified && !test->specified(argc, argv)) {
			continue;
		}
		current_filename = test->filename;
		current_line = test->line;
		++total_test_count;
		bool ok = true;
		std::string exception_text;
		std::string test_name = test->suite;
		test_name += std::string(test_name.empty() ? "" : " :: ") + test->name;
		try {
			test->run();
		} catch(const AssertException & e) {
			ok = false;
			exception_text = format("{0}:{1}: {2}", e.filename, e.line, e.what);
		} catch(const std::exception & e) {
			ok = false;
			exception_text = format("{0}:{1}: exception caught: {2}", test->filename, test->line, e.what());
		} catch(...) {
			ok = false;
			exception_text = format("{0}:{1}: unknown exception", test->filename, test->line);
		}
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
	return failed_tests;
}

}

}
