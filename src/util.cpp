#include "util.h"
#include <sstream>
#include <unistd.h>

namespace Chthon {

void delay(unsigned msec)
{
	usleep(msec * 1000);
}

void split(const std::string & s, std::vector<std::string> & tokens, char delimeter)
{
	std::stringstream in(s);
	std::string token;
	while(std::getline(in, token, delimeter)) {
		tokens.push_back(token);
	}
}

std::vector<std::string> split(const std::string & s, char delimeter)
{
	std::vector<std::string> tokens;
	split(s, tokens, delimeter);
	return tokens;
}

bool starts_with(const std::string & s, const std::string & start)
{
	if(s.length() < start.length()) {
		return false;
	}
	return s.compare(0, start.length(), start) == 0;
}

bool ends_with(const std::string & s, const std::string & end)
{
	if(s.length() < end.length()) {
		return false;
	}
	return s.compare(s.length() - end.length(), end.length(), end) == 0;
}

}

std::vector<std::string> & operator<<(std::vector<std::string> & out, const char * t)
{
	out.push_back(t);
	return out;
}

