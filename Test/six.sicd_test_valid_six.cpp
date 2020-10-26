#include "pch.h"

#include <filesystem>

#include <import/sys.h>

#include "sicd_Test.h"

namespace fs = std::filesystem;

static fs::path getNitfPath()
{
    const auto cwd = fs::current_path();
    const auto root_dir = cwd.parent_path().parent_path();
    return root_dir / "six" / "modules" / "c++" / "six" / "tests" / "nitf" / "sicd_50x50.nitf";
}

static void setNitfPluginPath()
{
    const auto cwd = fs::current_path();
    const auto root_dir = cwd.parent_path().parent_path().parent_path().parent_path().parent_path();

    const std::string configuration =
#if defined(NDEBUG) // i.e., release
        "Release";
#else
        "Debug";
#endif
    const std::string platform = "x64";

    const auto path = root_dir / "externals" / "nitro" / platform / configuration / "share" / "nitf" / "plugins";
    sys::OS().setEnv("NITF_PLUGIN_PATH", path.string(), true /*overwrite*/);
}

struct sicd_test_valid_six : public ::testing::Test {
    sicd_test_valid_six() {
        // initialization code here
        setNitfPluginPath();
        sys::OS().setEnv("SIX_UNIT_TEST_NITF_PATH_", getNitfPath().string(), true /*overwrite*/);
    }

    void SetUp() {
        // code here will execute just before the test ensues 
    }

    void TearDown() {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }

    ~sicd_test_valid_six() {
        // cleanup any pending stuff, but no exceptions allowed
    }

    // put in any custom data members that you need 
};


#define TEST_CASE(X) TEST_F(sicd_test_valid_six, X)
#include "six/modules/c++/six.sicd/unittests/test_valid_six.cpp"
