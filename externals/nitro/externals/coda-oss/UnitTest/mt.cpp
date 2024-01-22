#include "pch.h"
#include "CppUnitTest.h"

#include <mt/CriticalSection.h>
#include <mt/ThreadPlanner.h>
#include <mt/ThreadGroup.h>
#include <mt/BalancedRunnable1D.h>
#include <mt/CPUAffinityInitializer.h>
#include <mt/CPUAffinityThreadInitializer.h>
#include <mt/RequestQueue.h>
#include <mt/ThreadPoolException.h>
#include <mt/GenerationThreadPool.h>
#include <mt/ThreadedByteSwap.h>

namespace mt
{

TEST_CLASS(balanced_runnable_1d_test){ public:
#include "mt/unittests/balanced_runnable_1d_test.cpp"
};

TEST_CLASS(Runnable1DTest){ public:
#include "mt/unittests/Runnable1DTest.cpp"
};

TEST_CLASS(ThreadGroupTest){ public:
#include "mt/unittests/ThreadGroupTest.cpp"
};

TEST_CLASS(ThreadPlannerTest){ public:
#include "mt/unittests/ThreadPlannerTest.cpp"
};

TEST_CLASS(work_sharing_balanced_runnable_1d_test){ public:
#include "mt/unittests/work_sharing_balanced_runnable_1d_test.cpp"
};

TEST_CLASS(test_mt_byte_swap){ public:
#include "mt/unittests/test_mt_byte_swap.cpp"
};

}