#include "pch.h"
#include "CppUnitTest.h"

#include <avx/extractf.h>

namespace avx
{

TEST_CLASS(test_m256){ public:
#include "avx/unittests/test_m256.cpp"
};

}