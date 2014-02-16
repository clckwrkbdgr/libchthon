#include "files.h"
#include "util.h"
#include "map.h"
#include "format.h"
#include <sys/stat.h>

/** @ingroup Files
 * @page serialization Serialization using Files module
 *
 * Reader and Writer classes are designed for simple serialization of structures.
 *
 * @section intro Introduction
 *
 * Let's say we have this struct:
 *
 * @code{.cpp}
 * struct UserDefinedType {
 * 	int i;
 * 	char ch;
 * 	UserDefinedType() : i(0), ch(0) {}
 * 	UserDefinedType(int _i, char _ch) : i(_i), ch(_ch) {}
 * };
 * @endcode
 * 
 * In order to store user defined type in file, following two functions should be declared:
 *
 * @code{.cpp}
 * // For reading.
 * template<class Savefile>
 * void store(Savefile & savefile, Type & value, const char * section = nullptr);
 * // For writing.
 * template<class Savefile>
 * void store(Savefile & savefile, const Type & value, const char * section = nullptr)
 * @endcode
 * 
 * Templated Savefile parameter should be of Reader or Writer type.
 * Optional section parameter is used for section check() calls.
 * 
 * They both are used in the same way:
 * 
 * @code{.cpp}
 * UserDefinedType value(1, 'A');
 * store(reader, value);
 * @endcode
 *
 * @section store Store functions
 * 
 * There are predefined store() functions for some built-in types, currently are:
 * int, unsigned, char, bool, std::string. This functions also can be used as a member functions:
 *
 * @code{.cpp}
 * savefile.store(1);
 * @endcode
 * 
 * Usually (especially for POD structures) content of both reading and writing store()
 * fuctions are the same, so there is no need to write two functions. In that case,
 * SAVEFILE_STORE() macro should be used. It declares both functions for reading
 * writing using one definition:
 * 
 * @code{.cpp}
 * SAVEFILE_STORE(UserNamespace::UserDefinedType, value)
 * {
 * 	savefile.store(value.i).store(value.ch);
 * 	// This is section check.
 * 	// It will throw an exception if stream is bad.
 * 	savefile.check(section);
 * }
 * @endcode
 * 
 * Example of generic usage case of Writer:
 * 
 * @code{.cpp}
 * std::ostringstream out;
 * Writer writer(out);
 * store(writer, UserNamespace::UserDefinedType(1, 'A'));
 * @endcode
 * 
 * For reader it's the same.
 *
 * @section version Versioning streams
 * 
 * Both Reader and Writer have possibility to check versions of the stream.
 * The idea is that savefile could have version consisted of two parts: major and minor.
 * Major version is checked strictly: if reader major version is not the same as the savefile's,
 * exception is thrown. Minor versions has some indulgence: if minor version of the
 * savefile is greater than current application reader version, exception is thrown.
 * That way version() function could be used in the store() functions to determine
 * current minor version and do some stuff regarding it. For example, lets say we
 * definesd UsedDefinedType in version 1.0, and now in version 1.1 we're adding
 * a new std::string member:
 *
 * @code{.cpp}
 * struct UserDefinedType {
 * 	int i;
 * 	char ch;
 * 	std::string s;
 * 	...
 * };
 * @endcode
 *
 * In order to support both versions we should do the check in the store function():
 *
 * @code{.cpp}
 * SAVEFILE_STORE(UserNamespace::UserDefinedType, value)
 * {
 * 	savefile.store(value.i).store(value.ch);
 * 	if(savefile.version() >= 1) {
 * 		store(savefile, value.s);
 * 	}
 * 	savefile.check(section);
 * }
 * @endcode
 * 
 */

namespace Chthon {

bool file_exists(const std::string & filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0); 
}

std::string read_string(std::istream & in, char quote, char nonword)
{
	std::string result;
	char c;
	in >> std::ws;
	in.get(c);
	bool quoted = c == quote;
	if(quoted) {
		in.get(c);
	}
	while(in.good()) {
		bool done = (quoted ? (c == quote) : (isspace(c) || c == nonword));
		if(done) {
			break;
		}
		if(quoted && c == '\\') {
			in.get(c);
		}
		result += c;
		in.get(c);
	}
	return result;
}

std::string escaped(const std::string & s, char quote)
{
	std::string result = s;
	size_t pos = result.find(quote);
	while(pos != std::string::npos) {
		result.replace(pos, 1, std::string("\\") + quote);
		pos = result.find(quote, pos + 2);
	}
	return quote + result + quote;
}

Reader::Reader(std::istream & in_stream)
	: actual_minor_version(0), in(in_stream)
{
}

Reader & Reader::newline()
{
	return *this;
}

Reader & Reader::version(int major_version, int minor_version)
{
	int actual_major_version;
	in >> actual_major_version;
	check("major version");
	if(actual_major_version != major_version) {
		throw Exception(format(
					"Savefile has major version {0}, which is incompatible with current program savefile major version {1}.",
					actual_major_version, major_version
					));
	}
	in >> actual_minor_version;
	check("minor version");
	if(actual_minor_version > minor_version) {
		throw Exception(format(
					"Savefile has minor version {0}, which is incompatible with current program savefile minor version {1}.",
					actual_minor_version, minor_version
					));
	}
	return *this;
}

Reader & Reader::check(const std::string & section)
{
	if(!in.good()) {
		throw Exception("Error: savefile is corrupted (reading " + to_string(section) + ").");
	}
	return *this;
}

Reader & Reader::store(int & value)
{
	in >> value;
	return *this;
}

Reader & Reader::store(unsigned int & value)
{
	in >> value;
	return *this;
}

Reader & Reader::store(char & value)
{
	int stored_value;
	in >> stored_value;
	value = (char)stored_value;
	return *this;
}

Reader & Reader::store(bool & value)
{
	int stored_value;
	in >> stored_value;
	value = stored_value;
	return *this;
}

Reader & Reader::store(std::string & value)
{
	value = read_string(in);
	return *this;
}


Writer::Writer(std::ostream & out_stream)
	: actual_minor_version(0), out(out_stream)
{
}

Writer & Writer::newline()
{
	out << '\n';
	return *this;
}

Writer & Writer::version(int major_version, int minor_version)
{
	out << major_version << ' ';
	out << minor_version << ' ';
	actual_minor_version = minor_version;
	return *this;
}

Writer & Writer::check(const std::string & section)
{
	if(!out.good()) {
		throw Exception("Error: savefile is corrupted (writing " + to_string(section) + ") .");
	}
	return *this;
}

Writer & Writer::store(int value)
{
	out << value << ' ';
	return *this;
}

Writer & Writer::store(unsigned int value)
{
	out << value << ' ';
	return *this;
}

Writer & Writer::store(char value)
{
	out << int(value) << ' ';
	return *this;
}

Writer & Writer::store(bool value)
{
	out << int(value) << ' ';
	return *this;
}

Writer & Writer::store(const std::string & value)
{
	out << escaped(value) << ' ';
	return *this;
}

}
