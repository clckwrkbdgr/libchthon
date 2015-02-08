#include "point.h"
#include "format.h"
#include <cmath>

namespace Chthon {

template<> bool equal(const float & a, const float & b)
{ return float_equal(a, b); }
template<> bool equal(const double & a, const double & b)
{ return float_equal(a, b); }
template<> bool equal(const long double & a, const long double & b)
{ return float_equal(a, b); }

}
