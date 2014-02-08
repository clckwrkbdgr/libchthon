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
/** Overloaded logging routine for templated formatted output.
 * @see log()
 * @see format()
 */
template<class... Args>
void log(const std::string & message, Args... args)
{
	log(format(message, args...));
}

/// Prints expression text and its value to log.
#define TRACE(x) do { Chthon::log(Chthon::format(#x "={0}", (x))); } while(0)

/// @}
}
