#include "format.h"
#include <sstream>

namespace Chthon {

std::string to_string(bool value)
{
	static const std::string t = "true", f = "false";
	return value ? t : f;
}

std::string to_string(int value)
{
	return std::to_string(value);
}

std::string to_string(char value)
{
	return std::string(1, value);
}

std::string to_string(unsigned value)
{
	return std::to_string(value);
}

std::string to_string(long unsigned value)
{
	return std::to_string(value);
}

std::string to_string(const std::string & value)
{
    return value;
}

std::string to_string(const char * value)
{
	return value;
}


/// @cond INTERNAL
Spec::Spec(const std::string & _spec)
	: width(0), fill(' '), spec(_spec)
{
	size_t last_digits = spec.find_last_not_of("0123456789");
	if(last_digits == std::string::npos) {
		last_digits = 0;
	}
	width_start = spec.find_first_of("123456879", last_digits);
	if(width_start != std::string::npos) {
		width = std::stoul(spec.substr(width_start));
		if(width_start > 0) {
			fill = spec[width_start - 1];
			--width_start;
		}
	}
}

std::string Spec::flags() const
{
	return spec.substr(0, width_start);
}

std::string Spec::padded(const std::string & subs) const
{
	if(subs.size() < width) {
		return std::string(width - subs.size(), fill) + subs;
	}
	return subs;
}

Format::Format(std::string & _result, int index)
	: result(_result), placeholder_index(std::to_string(index)), pos(0), end(0)
{
}

bool Format::ok() const
{
	return pos != std::string::npos && end != std::string::npos;
}

bool Format::to_next()
{
	spec.clear();
	while(ok()) {
		pos = result.find('{', end);
		if(!ok()) {
			return false;
		}
		end = result.find('}', pos);
		if(!ok()) {
			return false;
		}
		if(result.substr(pos + 1, placeholder_index.size()) == placeholder_index) {
			size_t spec_pos = result.find(':', pos);
			if(spec_pos < end) {
				spec = result.substr(spec_pos + 1, end - spec_pos - 1);
			}
			return true;
		}
	}
	return false;
}

const std::string & Format::get_spec() const
{
	return spec;
}

void Format::replace_current(const std::string & value)
{
	result.replace(pos, end - pos + 1, value);
}

template<class T>
std::string int_to_string_with_flags(const T & value, const std::string & flags)
{
	if(flags.find('#') != std::string::npos) {
		std::ostringstream stream;
		stream << std::hex << value;
		return stream.str();
	}
	return to_string(value);
}

std::string to_string_with_flags(int value, const std::string & flags)
{
	/// Flag `#` commands to convert value to hex.
	return int_to_string_with_flags(value, flags);
}

std::string to_string_with_flags(unsigned value, const std::string & flags)
{
	/// Flag `#` commands to convert value to hex.
	return int_to_string_with_flags(value, flags);
}

std::string to_string_with_flags(uint8_t value, const std::string & flags)
{
	/// Flag `#` commands to convert value to hex.
	return int_to_string_with_flags(value, flags);
}

void subs_args_from(std::string &, int)
{
}
/// @endcond

}
