#include "pch.h"

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(test_image_loading__) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_image_loading++.cpp"

};
std::string test_image_loading__::argv0;
