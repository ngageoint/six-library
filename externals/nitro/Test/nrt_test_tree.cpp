#include "pch.h"

#include "nrt_Test.h"

const char* testName = "testTree";
#define TEST_CASE(X) TEST(nrt_test_tree, X)
#include "nrt/unittests/test_tree.c"
