#include "pch.h"

#include <import/sys.h>

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(test_tre_read) {
public:
	test_tre_read() {
		// initialization code here
		sys::OS().setEnv("NITF_PLUGIN_PATH", nitf::Test::buildPluginsDir(), true /*overwrite*/);
	}
	~test_tre_read() = default;
	test_tre_read(const test_tre_read&) = delete;
	test_tre_read& operator=(const test_tre_read&) = delete;

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_tre_read.cpp"

};
