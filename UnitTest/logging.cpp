#include "pch.h"
#include "CppUnitTest.h"

namespace logging
{

TEST_CLASS(test_exception_logger){ public:
#include "logging/unittests/test_exception_logger.cpp"
};

TEST_CLASS(test_rotating_log){ public:
#include "logging/unittests/test_rotating_log.cpp"
};

}