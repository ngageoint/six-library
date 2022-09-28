#include "pch.h"

#include "nrt_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nrt_test_utils) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nrt/unittests/test_utils.c"

};