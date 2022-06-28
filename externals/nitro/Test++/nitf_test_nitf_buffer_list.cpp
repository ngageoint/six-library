#include "pch.h"

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(test_nitf_buffer_list) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_nitf_buffer_list.cpp"

};