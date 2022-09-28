#include "pch.h"

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(test_j2k_read_tile) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_j2k_read_tile.cpp"

};