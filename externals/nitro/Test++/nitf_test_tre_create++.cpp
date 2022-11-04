#include "pch.h"

#include <import/sys.h>

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nitf_test_tre_create__) {
public:
	nitf_test_tre_create__() {
		// initialization code here
		nitf::Test::setNitfPluginPath();
	}
	~nitf_test_tre_create__() = default;
	nitf_test_tre_create__(const nitf_test_tre_create__&) = delete;
	nitf_test_tre_create__& operator=(const nitf_test_tre_create__&) = delete;

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_tre_create++.cpp"

};