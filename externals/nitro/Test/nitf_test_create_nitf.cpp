#include "pch.h"

#include "nitf_Test.h"

#define TEST_NAME nitf_test_create_nitf
#define TEST_CASE(X) TEST(TEST_NAME, X)

int argc = 1;
const char* argv[] = { "nitf_test_create_nitf" };
#define TEST_CASE_ARGS(X) TEST(TEST_NAME, X)

#include "nitf/unittests/test_create_nitf.c"
