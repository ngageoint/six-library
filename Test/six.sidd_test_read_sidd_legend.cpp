#include "pch.h"

#include "sidd_Test.h"

struct sidd_test_read_sidd_legend : public ::testing::Test {
    sidd_test_read_sidd_legend() {
        // initialization code here
        const std::string SIX_SCHEMA_PATH = R"(C:\Users\jdsmith\source\repos\six\six\modules\c++\six.sidd\conf\schema)";
        const std::string putenv_ = "SIX_SCHEMA_PATH=" + SIX_SCHEMA_PATH;

        _putenv(putenv_.c_str());
    }

    void SetUp() {
        // code here will execute just before the test ensues 
    }

    void TearDown() {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }

    ~sidd_test_read_sidd_legend() {
        // cleanup any pending stuff, but no exceptions allowed
    }

    // put in any custom data members that you need 
};

#define TEST_CASE(X) TEST_F(sidd_test_read_sidd_legend, X)
#include "six/modules/c++/six.sidd/unittests/test_read_sidd_legend.cpp"
