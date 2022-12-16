#include "pch.h"

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(test_j2k_compressed_byte_provider) {
public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_j2k_compressed_byte_provider.cpp"

};

std::string test_j2k_compressed_byte_provider::testName;
