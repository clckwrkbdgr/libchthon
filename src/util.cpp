#include "util.h"
#include <unistd.h>

namespace Chthon {

void delay(unsigned msec)
{
	usleep(msec * 1000);
}

}

std::vector<std::string> & operator<<(std::vector<std::string> & out, const char * t)
{
	out.push_back(t);
	return out;
}

