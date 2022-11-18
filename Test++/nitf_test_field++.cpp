#include "pch.h"

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nitf_test_field__) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_field++.cpp"

};