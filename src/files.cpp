#include "files.h"
#include "util.h"
#include "map.h"
#include "format.h"
#include <sys/stat.h>

namespace Chthon {

bool file_exists(const std::string & filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0); 
}

std::string read_string(std::istream & in, char quote)
{
	std::string result;
	char c;
	in >> std::ws;
	in.get(c);
	if(c != quote) {
		return result;
	}
	in.get(c);
	while(in.good() && c != quote) {
		if(c == '\\') {
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
