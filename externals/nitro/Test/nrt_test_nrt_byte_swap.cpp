#include "pch.h"

#include "nrt_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nrt_test_nrt_byte_swap) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nrt/unittests/test_nrt_byte_swap.c"

};
