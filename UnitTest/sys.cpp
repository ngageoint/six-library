#include "pch.h"
#include "CppUnitTest.h"

#include <import/sys.h>
#include <sys/Runnable.h>
#include <sys/Thread.h>
#include <sys/Mutex.h>
#include <sys/FileFinder.h>
#include <sys/ByteSwap.h>
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
#include <sys/Conf.h>
#include <sys/Path.h>

namespace sys
{
TEST_CLASS(test_aligned_alloc){ public:
#include "sys/unittests/test_aligned_alloc.cpp"
};

TEST_CLASS(test_atomic_counter){ public:
#include "sys/unittests/test_atomic_counter.cpp"
};

TEST_CLASS(test_byte_swap){ public:
#include "sys/unittests/test_byte_swap.cpp"
};

TEST_CLASS(test_conditionvar){ public:
#include "sys/unittests/test_conditionvar.cpp"
};

TEST_CLASS(test_datetime){ public:
#include "sys/unittests/test_datetime.cpp"
};

TEST_CLASS(test_NaN_testing){ public:
#include "sys/unittests/test_NaN_testing.cpp"
};

TEST_CLASS(test_os){ public:
#include "sys/unittests/test_os.cpp"
};

TEST_CLASS(test_path){ public:
#include "sys/unittests/test_path.cpp"
};

}