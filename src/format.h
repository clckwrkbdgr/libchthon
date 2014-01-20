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
/// Replace placeholder with value using corresponding `to_string` function.
template<class T>
void subs_arg(std::string & result, int index, const T & value) { subs_arg_str(result, index, to_string(value)); }
/// @endcond

/** Formats string using placeholder substitution.
 * If there is no `{0}` placeholder, original string returns intact.
 * @param s original string with placeholders.
 * @param t0 value to replace `{0}` placeholder.
 * @return formatted string.
 *
 * Example code:
 * @code{.cpp}
 * format("Hello {0}!", "world") -> "Hello, world"
 * @endcode
 */
template<class T0>
	std::string format(std::string s, const T0 & t0)
	{ subs_arg(s, 0, t0); return s; }
/** Formats string using placeholder substitution.
 * @see format().
 * @param s original string with placeholders.
 * @param t0 value to replace `{0}` placeholder.
 * @param t1 value to replace `{1}` placeholder.
 * @return formatted string.
 */
template<class T0, class T1>
	std::string format(std::string s, const T0 & t0, const T1 & t1)
	{ subs_arg(s, 0, t0); subs_arg(s, 1, t1); return s; }
/** Formats string using placeholder substitution.
 * @see format().
 * @param s original string with placeholders.
 * @param t0 value to replace `{0}` placeholder.
 * @param t1 value to replace `{1}` placeholder.
 * @param t2 value to replace `{2}` placeholder.
 * @return formatted string.
 */
template<class T0, class T1, class T2>
	std::string format(std::string s, const T0 & t0, const T1 & t1, const T2 & t2)
	{ subs_arg(s, 0, t0); subs_arg(s, 1, t1); subs_arg(s, 2, t2); return s; }
/** Formats string using placeholder substitution.
 * @see format().
 * @param s original string with placeholders.
 * @param t0 value to replace `{0}` placeholder.
 * @param t1 value to replace `{1}` placeholder.
 * @param t2 value to replace `{2}` placeholder.
 * @param t3 value to replace `{3}` placeholder.
 * @param t4 value to replace `{4}` placeholder.
 * @return formatted string.
 */
template<class T0, class T1, class T2, class T3, class T4>
	std::string format(std::string s, const T0 & t0, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4)
	{ subs_arg(s, 0, t0); subs_arg(s, 1, t1); subs_arg(s, 2, t2); subs_arg(s, 3, t3); subs_arg(s, 4, t4); return s; }

/// @}
}
