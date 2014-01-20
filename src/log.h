#pragma once
#include "format.h"

namespace Chthon { /// @defgroup Log Logging utilities
/// @{

/// Current date and time as string.
std::string now();

/// Redirects log to specified stream.
void direct_log(std::ostream * out_stream);
/// Outputs message to log.
void log(const std::string & message);
/** Overloaded logging routine for formatted output.
 * @see log()
 * @see format()
 */
template<class T0>
	void log(const std::string & message, const T0 & t0)
	{ log(format(message, t0)); }
/** Overloaded logging routine for formatted output.
 * @see log()
 * @see format()
 */
template<class T0, class T1>
	void log(const std::string & message, const T0 & t0, const T1 & t1)
	{ log(format(message, t0, t1)); }
/** Overloaded logging routine for formatted output.
 * @see log()
 * @see format()
 */
template<class T0, class T1, class T2>
	void log(const std::string & message, const T0 & t0, const T1 & t1, const T2 & t2)
	{ log(format(message, t0, t1, t2)); }

/// Prints expression text and its value to log.
#define TRACE(x) do { log(format(#x "={0}", (x))); } while(0);

/// @}
}
