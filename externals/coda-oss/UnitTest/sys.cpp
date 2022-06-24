#include "pch.h"
#include "CppUnitTest.h"

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