#include "pch.h"
#include "CppUnitTest.h"

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

}