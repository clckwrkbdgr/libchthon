#include "util.h"
#include "log.h"
#include <sstream>
#include <unistd.h>
#include <cstring>

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

struct IsSpace {
	IsSpace(const std::string & whitespace_string)
		: ws(whitespace_string), specified(!ws.empty())
	{}
	bool operator()(char c) const
	{
		return specified ? contains(ws, c) : isspace(c);
	}
private:
	const std::string & ws;
	bool specified;
};

std::string collapse_whitespaces(const std::string & s,
		const std::string & whitespace)
{
	if(s.empty()) {
		return s;
	}
	std::string result;
	IsSpace is_space(whitespace);
	bool is_space_group = false;
	for(char c : s) {
		if(is_space(c)) {
			if(!is_space_group) {
				result += ' ';
				is_space_group = true;
			}
		} else {
			result += c;
			is_space_group = false;
		}
	}
	return result;
}

std::string trim_left(const std::string & s, const std::string & whitespace)
{
	if(s.empty()) {
		return s;
	}
	IsSpace is_space(whitespace);
	auto end = std::find_if_not(s.begin(), s.end(), is_space);
	if(end == s.end()) {
		return "";
	}
	return s.substr(size_t(end - s.begin()));
}

std::string trim_right(const std::string & s, const std::string & whitespace)
{
	if(s.empty()) {
		return s;
	}
	IsSpace is_space(whitespace);
	auto end = std::find_if_not(s.rbegin(), s.rend(), is_space);
	if(end == s.rend()) {
		return "";
	}
	size_t index = size_t(end - s.rbegin());
	return s.substr(0, s.length() - index);
}

std::string trim(const std::string & s, const std::string & whitespace)
{
	if(s.empty()) {
		return s;
	}
	IsSpace is_space(whitespace);
	auto begin = std::find_if_not(s.begin(), s.end(), is_space);
	if(begin == s.end()) {
		return "";
	}
	auto end = std::find_if_not(s.rbegin(), s.rend(), is_space);
	size_t left = size_t(begin - s.begin());
	size_t right = size_t(end - s.rbegin());
	return s.substr(left, s.length() - right - left);
}

bool contains(const std::string & s, const std::string & pattern)
{
	return s.find(pattern) != std::string::npos;
}

bool contains(const std::string & s, char c)
{
	return s.find(c) != std::string::npos;
}

bool contains(const char * s, const char * pattern)
{
	return strstr(s, pattern) != nullptr;
}

bool contains(const char * s, char c)
{
	return strchr(s, c) != nullptr;
}

}

namespace Chthon { // InterleavedCharMap

InterleavedCharMap::const_iterator InterleavedCharMap::begin(size_t index) const
{
	return const_iterator(index, *this, data[index].begin());
}

InterleavedCharMap::const_iterator InterleavedCharMap::end(size_t index) const
{
	return const_iterator(index, *this, data[index + (height - 1) * count].end());
}

std::string InterleavedCharMap::value(size_t index) const
{
	return std::string(begin(index), end(index));
}

InterleavedCharMap::const_iterator::const_iterator(size_t map_index,
		const InterleavedCharMap & _map, const std::string::const_iterator & actual_it)
	: map(_map), index(map_index), it(actual_it),
	current_string(std::begin(map.data) + int(index))
{
}

InterleavedCharMap::const_iterator & InterleavedCharMap::const_iterator::operator++()
{
	++it;
	if(it == current_string->end()) {
		size_t count = map.count;
		std::vector<std::string>::const_iterator new_string = current_string;
		while(count --> 0 && new_string != std::end(map.data)) {
			++new_string;
		}
		if(new_string != std::end(map.data)) {
			current_string = new_string;
			it = current_string->begin();
		}
	}
	return *this;
}

InterleavedCharMap::const_iterator InterleavedCharMap::const_iterator::operator++(int)
{
	const_iterator value = *this;
	operator++();
	return value;
}

bool InterleavedCharMap::const_iterator::operator==(const const_iterator & other) const
{
	return index == other.index && it == other.it;
}

bool InterleavedCharMap::const_iterator::operator!=(const const_iterator & other) const
{
	return !operator==(other);
}

char InterleavedCharMap::const_iterator::operator*() const
{
	return *it;
}

char InterleavedCharMap::const_iterator::operator->() const
{
	return *it;
}

}

std::vector<std::string> & operator<<(std::vector<std::string> & out, const char * t)
{
	out.push_back(t);
	return out;
}

