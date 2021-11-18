#include "pch.h"

#include <sys/OS.h>

#include "sidd_Test.h"
#include "Test.h"

struct sidd_test_byte_provider : public ::testing::Test {
	sidd_test_byte_provider() {
        // initialization code here
        //const std::string SIX_SCHEMA_PATH = R"(C:\Users\jdsmith\source\repos\six\six\modules\c++\six.sidd\conf\schema)";
		const auto SIX_SCHEMA_PATH = six::Test::buildSchemaDir();
		sys::OS().setEnv("SIX_SCHEMA_PATH", SIX_SCHEMA_PATH.string(), true /*overwrite*/);
    }

    void SetUp() {
        // code here will execute just before the test ensues 
    }

    void TearDown() {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }

    ~sidd_test_byte_provider() {
        // cleanup any pending stuff, but no exceptions allowed
    }

    // put in any custom data members that you need 
};

#define TEST_CASE(X) TEST_F(sidd_test_byte_provider, X)
#include "six/modules/c++/six.sidd/unittests/test_sidd_byte_provider.cpp"
