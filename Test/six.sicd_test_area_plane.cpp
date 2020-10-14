#include "pch.h"

#include "sicd_Test.h"

struct sicd_test_area_plane : public ::testing::Test {
    sicd_test_area_plane() {
        // initialization code here
        const std::string putenv_ = "NITF_PLUGIN_PATH=" NITF_PLUGIN_PATH;
        _putenv(putenv_.c_str());
    }

    void SetUp() {
        // code here will execute just before the test ensues 
    }

    void TearDown() {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }

    ~sicd_test_area_plane() {
        // cleanup any pending stuff, but no exceptions allowed
    }

    // put in any custom data members that you need 
};


#define TEST_CASE(X) TEST_F(sicd_test_area_plane, X)
#include "six/modules/c++/six.sicd/unittests/test_area_plane.cpp"
