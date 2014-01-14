#include "format.h"
#include <sstream>

namespace Chthon {

std::string to_string(int value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(char value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(unsigned value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(long unsigned value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(const std::string & value)
{
    return value;
}

std::string to_string(const char * value)
{
	return value;
}

void subs_arg_str(std::string & result, int index, const std::string & value)
{
	std::ostringstream out;
	out << "{" << index << "}";
	std::string placeholder = out.str();
	size_t pos = result.find(placeholder);
	while(pos != std::string::npos) {
		result.replace(pos, placeholder.size(), value);
		pos = result.find(placeholder, pos + 1);
	}
}

}
