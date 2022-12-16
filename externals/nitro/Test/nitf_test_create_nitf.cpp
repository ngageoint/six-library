#include "pch.h"

#include "nitf_Test.h"

#define TEST_NAME nitf_test_create_nitf
int argc = 1;
const char* argv[] = { "nitf_test_create_nitf" };

#include "nitf/unittests/nitro_image_.c_"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nitf_test_create_nitf) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#define TEST_CASE_ARGS(X) TEST_METHOD(X)
#include "nitf/unittests/test_create_nitf.c"

};