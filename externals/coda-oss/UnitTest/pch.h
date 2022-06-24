// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <WinSock.h>
#include <windows.h>
#undef min
#undef max

#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include <typeinfo>
#include <locale>
#include <limits>
#include <iterator>
#include <array>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <std/string>
#include <std/span>
#include <std/optional>
#include <std/cstddef>
#include <std/bit>
#include <algorithm>
#include <std/bit> // std::endian
#include <std/cstddef>
#include <numeric> // std::accumulate
#include <std/filesystem>
#include <future>
#include <cassert>
#include <tuple>

#include "CppUnitTest.h"

#include "gsl/gsl.h"
#include "import/sys.h"
#include "import/math.h"
#include "import/str.h"
#include "str/EncodedStringView.h"
#include "import/except.h"
#include "import/mem.h"
#include <mem/SharedPtr.h>
#include "import/cli.h"
#include "polygon/DrawPolygon.h"
#include "polygon/PolygonMask.h"
#include <types/RowCol.h>
#include <types/PageRowCol.h>
#include <types/RangeList.h>
#include <types/Range.h>
#include <sys/Conf.h>
#include <sys/Path.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <sys/AtomicCounter.h>
#include <sys/Runnable.h>
#include <sys/Thread.h>
#include <sys/Conf.h>
#include <sys/ConditionVar.h>
#include <sys/LocalDateTime.h>
#include <sys/UTCDateTime.h>
#include <sys/OS.h>
#include <sys/Path.h>
#include <sys/Backtrace.h>
#include <sys/Dbg.h>
#include <sys/DateTime.h>
#include <sys/sys_filesystem.h>
#include <except/Error.h>
#include <sys/Runnable.h>
#include <sys/Thread.h>
#include <sys/Mutex.h>
#include <import/sys.h>
#include <import/math/linear.h>
#include <import/math/poly.h>
#include <mt/CriticalSection.h>
#include <mt/ThreadPlanner.h>
#include <mt/ThreadGroup.h>
#include <mt/BalancedRunnable1D.h>
#include <mt/CPUAffinityInitializer.h>
#include <mt/CPUAffinityThreadInitializer.h>
#include <mt/RequestQueue.h>
#include <mt/ThreadPoolException.h>
#include <mt/GenerationThreadPool.h>
#include <import/mt.h>
#include <avx/extractf.h>
#include <logging/ExceptionLogger.h>
#include <import/logging.h>
#include <import/re.h>

#include "TestCase.h"

#endif //PCH_H
