#pragma once
#include "format.h"

namespace Chthon {

std::string now();

void direct_log(std::ostream * out_stream);
void start_log(const std::string & filename);
void log(const std::string & message);
template<class T0>
	void log(const std::string & message, const T0 & t0)
	{ log(format(message, t0)); }
template<class T0, class T1>
	void log(const std::string & message, const T0 & t0, const T1 & t1)
	{ log(format(message, t0, t1)); }
template<class T0, class T1, class T2>
	void log(const std::string & message, const T0 & t0, const T1 & t1, const T2 & t2)
	{ log(format(message, t0, t1, t2)); }

#define TRACE(x) do { log(format(#x "={0}", (x))); } while(0);

}
