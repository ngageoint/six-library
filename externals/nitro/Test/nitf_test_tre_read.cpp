#include "pch.h"

#include <import/sys.h>

#include "nitf_Test.h"

struct test_tre_read : public ::testing::Test {
	test_tre_read() {
		// initialization code here
		//const std::string NITF_PLUGIN_PATH = R"(C:\Users\jdsmith\source\repos\nitro\x64\Debug\share\nitf\plugins)";
		nitf::Test::setNitfPluginPath();
	}

	void SetUp() {
		// code here will execute just before the test ensues 
	}

	void TearDown() {
		// code here will be called just after the test completes
		// ok to through exceptions from here if need be
	}

	~test_tre_read() {
		// cleanup any pending stuff, but no exceptions allowed
	}

	test_tre_read(const test_tre_read&) = delete;
	test_tre_read& operator=(const test_tre_read&) = delete;


	// put in any custom data members that you need 
};

#define TEST_CASE(X) TEST_F(test_tre_read, X)
#include "nitf/unittests/test_tre_read.cpp"
