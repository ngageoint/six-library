#include "pch.h"

#include <import/sys.h>

#include "nitf_Test.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nitf_test_writer_3__) {
public:
	nitf_test_writer_3__() {
		// initialization code here
		sys::OS().setEnv("NITF_PLUGIN_PATH", nitf::Test::buildPluginsDir(), true /*overwrite*/);
	}
	~nitf_test_writer_3__() = default;
	nitf_test_writer_3__(const nitf_test_writer_3__&) = delete;
	nitf_test_writer_3__& operator=(const nitf_test_writer_3__&) = delete;

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_writer_3++.cpp"

};
std::string nitf_test_writer_3__::argv0;