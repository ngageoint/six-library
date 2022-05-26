#include "pch.h"

#include "cphd03_Test.h"

#define CODA_OSS_testCase_(X)  testCase ## _ ## X
#define TEST_CASE(X) void CODA_OSS_testCase_(X)(std::string); TEST(cphd03_test_vbm, X) { CODA_OSS_testCase_(X)(#X); } void CODA_OSS_testCase_(X)(std::string testName)
#include "six/modules/c++/cphd03/unittests/test_vbm.cpp"
