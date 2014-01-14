#pragma once
#include <string>
#include <vector>

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION < 40300
// Ugly hack for decltype presence.
#define decltype __typeof__
#endif

namespace Chthon {

void delay(int msec);

template<typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

template<class T>
struct ForeachIterator {
	T iterator;
	bool done;
	bool brk;
	ForeachIterator(T t): iterator(t), done(false), brk(false)  { }
	operator bool() const { return true; }
	bool is_done() { if(!done) { brk = true; };  return done; }
	void mark_done() { brk = false; done = true;  }
	void reset_done() { done = false;  }
};
#define foreach(expression, container) \
	if(ForeachIterator<decltype(container.begin())> it = container.begin()) \
        for(; it.iterator != container.end() && !it.brk; ++it.iterator, it.reset_done()) \
            for(expression = *it.iterator; !it.is_done(); it.mark_done())

template<class T>
std::vector<T> & operator<<(std::vector<T> & out, const T & t)
{
	out.push_back(t);
	return out;
}
std::vector<std::string> & operator<<(std::vector<std::string> & out, const char * t);

template<class T, size_t N>
size_t size_of_array(T (&)[N]) { return N; }

}
