#include "log.h"
#include "format.h"
#include <iostream>

namespace Chthon {

std::string now()
{
	time_t t = time(nullptr);
	/// Formatting is done using `ctime` function.
	std::string s = ctime(&t);
	/// Result string has got no trailing newlines.
	return s.replace(s.find('\n'), 1, " ");
}

static void log_control(std::ostream * out_stream, const std::string & message)
{
	static std::ostream * out = &std::clog;
	if(out_stream) {
		out = out_stream;
	}
	if(out && !message.empty()) {
		*out << message << std::endl;
	}
}

/// If `out_stream` is null, stdlog is used (behaviour by default).
/// After redirecting is done, message "Log started: <current_time>" is printed.
void direct_log(std::ostream * out_stream)
{
	log_control(out_stream, "Log started: " + now());
}

/// Each message is appended with newline.
void log(const std::string & message)
{
	log_control(nullptr, message);
}

}
