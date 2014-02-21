#pragma once
#include <string>
#include <iostream>
#include <sstream>

namespace Chthon { /// @defgroup Format String formatting utilities
/// @{

/// Converts boolean value to string ("true"/"false").
std::string to_string(bool value);
/// Converts integer value to string.
std::string to_string(int value);
/// Converts unsigned integer value to string.
std::string to_string(unsigned value);
/// Converts unsigned long integer value to string.
std::string to_string(long unsigned value);
/// Converts char value to string.
std::string to_string(char value);
/// Converts std::string value to string (does nothing, basically).
std::string to_string(const std::string & value);
/// Converts c-string value to string (does nothing, basically).
std::string to_string(const char * value);
/// Converts pointer to an integer value of its address.
template<class T>
std::string to_string(const T * value)
{
	std::ostringstream out;
	out << std::hex << value;
	return out.str();
}

/// @cond INTERNAL
/// Class for processing of format specificator.
struct Spec {
	Spec(const std::string & _spec);
	/// Return flags of the specificator (without width and fill).
	std::string flags() const;
	/// Commit padding of the given subs string.
	std::string padded(const std::string & subs) const;
private:
	size_t width_start;
	size_t width;
	char fill;
	const std::string & spec;
};
/// Class for processing single argument.
struct Format {
	Format(std::string & _result, int index);
	/// Current state.
	bool ok() const;
	/// Moves to the next placeholder and prepares spec string.
	bool to_next();
	const std::string & get_spec() const;
	/// Commit replacement of the given value string representation.
	void replace_current(const std::string & value);
private:
	std::string & result;
	std::string placeholder_index;
	std::string spec;
	size_t pos, end;
};
/// Basic generic to_string conversion using flags.
/// Default behaviour is to ignore flags.
template<class T>
std::string to_string_with_flags(const T & value, const std::string & /*flags*/)
{
	return to_string(value);
}
/// To_string conversion for integer.
std::string to_string_with_flags(int value, const std::string & flags);

/// Replace placeholder {X} (X is index, i.e. {0}, {1}, ...) with value.
void subs_arg_str(std::string & result, int index, const std::string & value);
/// Default case for recursive variadic template search.
void subs_args_from(std::string &, int);
/// Replace placeholder with value using corresponding `to_string` function.
template<class T, class... Args>
void subs_args_from(std::string & result, int index, const T & value, Args... args)
{
	Format f(result, index);
	while(f.to_next()) {
		if(f.get_spec().empty()) {
			f.replace_current(to_string(value));
		} else {
			Spec s(f.get_spec());
			f.replace_current(
					s.padded(to_string_with_flags(value, s.flags()))
					);
		}
	}
	subs_args_from(result, index + 1, args...);
}
/// @endcond

/** Formats string using placeholder substitution.
 * Each placeholder is replaced with resp. positional argument.
 * Placeholder should be in form: `{i[:[flags][[fill]width]]}`, where:
 * - **i** - position of the argument in the argument list;
 * - **flags** - conversion flags:
 *   - `#` converts integer value to hex.
 * - **fill** - fill character, used when argument size is more than specified
 *   width (space by default). Padded to the left.
 * - **width** - minimal width of the argument.
 * If there is no placeholders, original string is returned intact.
 * @param s original string with placeholders.
 * @param args values to replace `{i}` placeholder.
 * @return formatted string.
 *
 * Example code:
 * @code{.cpp}
 * format("Hello {0}!", "world") -> "Hello, world!"
 * format("{1} {0}!", "world", "Hello") -> "Hello, world!"
 * format("Hello {0}!") -> "Hello, {0}!"
 * @endcode
 */
template<class... Args>
std::string format(std::string s, Args... args)
{
	subs_args_from(s, 0, args...);
	return s;
}

/// @}
}
