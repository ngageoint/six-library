#include "pch.h"

#include "nrt_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const char* testName = "testTree";
TEST_CLASS(nrt_test_tree) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nrt/unittests/test_tree.c"

};