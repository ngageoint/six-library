#pragma once

#pragma warning(push)
#pragma warning(disable: 5220) // '...': a non - static data member with a volatile qualified type no longer implies
#pragma warning(disable: 5204) // 'Concurrency::details::_DefaultPPLTaskScheduler': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
#include <assert.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include <stddef.h>

#include <string>
#include <numeric>
#include <limits>
#include <memory>
#include <algorithm>
#include <sstream>
#include <ostream>
#include <iostream>
#include <ios>
#include <iomanip>
#include <vector>
#include <map>
#include <utility>
#include <functional>
#include <mutex>
#include <atomic>
#include <future>
#include <complex>

#include <windows.h>
#undef min
#undef max
#pragma warning(pop)