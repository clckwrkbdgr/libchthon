#include "log.h"
#include "format.h"
#include <iostream>

namespace Chthon {

std::string now()
{
	time_t t = time(0);
	std::string s = ctime(&t);
	return s.replace(s.find('\n'), 1, " ");
}

void log_control(std::ostream * out_stream, const std::string & message)
{
	static std::ostream * out = &std::clog;
	if(out_stream) {
		out = out_stream;
	}
	if(out && !message.empty()) {
		*out << message << std::endl;
	}
}

void direct_log(std::ostream * out_stream)
{
	log_control(out_stream, "Log started: " + now());
}

void log(const std::string & message)
{
	log_control(0, message);
}

}
