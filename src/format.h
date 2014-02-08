#pragma once
#include <string>

namespace Chthon { /// @defgroup Format String formatting utilities
/// @{

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

/// @cond INTERNAL
/// Replace placeholder {X} (X is index, i.e. {0}, {1}, ...) with value.
void subs_arg_str(std::string & result, int index, const std::string & value);
/// Default case for recursive variadic template search.
void subs_args_from(std::string &, int);
/// Replace placeholder with value using corresponding `to_string` function.
template<class T, class... Args>
void subs_args_from(std::string & result, int index, const T & value, Args... args)
{
	subs_arg_str(result, index, to_string(value));
	subs_args_from(result, index + 1, args...);
}
/// @endcond

/** Formats string using placeholder substitution.
 * Each `{i}` placeholder is replaced with resp. positional argument.
 * If there is no `{i}` placeholders, original string is returned intact.
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
