#pragma once
#include "monsters.h"
#include "cell.h"
#include "objects.h"
#include "util.h"
#include <fstream>
#include <vector>
#include <map>

namespace Chthon {

bool file_exists(const std::string & filename);
std::string read_string(std::istream & in, char quote = '"');
std::string escaped(const std::string & s, char quote = '"');

#define FORWARD_DECLARE_SAVEFILE_STORE(Type) \
	template<class Savefile> void store(Savefile &, Type &, const char * section = nullptr); \
	template<class Savefile> void store(Savefile &, const Type &, const char * section = nullptr)

#define SAVEFILE_STORE(Type, variable) \
	template<class Savefile, class T> void store_ext_##variable(Savefile &, T &); \
	template<class Savefile> void store(Savefile & savefile, Type & variable, const char * section = nullptr) \
		{ store_ext_##variable(savefile, variable); if(section) { savefile.check(section); } } \
	template<class Savefile> void store(Savefile & savefile, const Type & variable, const char * section = nullptr) \
		{ store_ext_##variable(savefile, variable); if(section) { savefile.check(section); } } \
	template<class Savefile, class T> \
	void store_ext_##variable(Savefile & savefile, T & variable)

SAVEFILE_STORE(int, int_value) { savefile.store(int_value); }
SAVEFILE_STORE(unsigned int, unsigned_int_value) { savefile.store(unsigned_int_value); }
SAVEFILE_STORE(char, char_value) { savefile.store(char_value); }
SAVEFILE_STORE(bool, bool_value) { savefile.store(bool_value); }
SAVEFILE_STORE(std::string, string_value) { savefile.store(string_value); }

class Reader {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Reader(std::istream & in_stream);

	Reader & newline();
	Reader & version(int major_version, int minor_version);
	int version() const { return actual_minor_version; }
	Reader & check(const std::string & section);
	
	Reader & store(int & value);
	Reader & store(unsigned int & value);
	Reader & store(char & value);
	Reader & store(bool & value);
	Reader & store(std::string & value);
private:
	int actual_minor_version;
	std::istream & in;
};

class Writer {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Writer(std::ostream & out_stream);

	Writer & newline();
	Writer & version(int major_version, int minor_version);
	int version() const { return actual_minor_version; }
	Writer & check(const std::string & section);

	Writer & store(int value);
	Writer & store(unsigned int value);
	Writer & store(char value);
	Writer & store(bool value);
	Writer & store(const std::string & value);
private:
	int actual_minor_version;
	std::ostream & out;
};

}
