#include "pch.h"

#include <sys/Filesystem.h>
#include <sys/OS.h>

#include "sidd_Test.h"

namespace fs = std::filesystem;

static bool is_x64_Configuration(const fs::path& path) // "Configuration" is typically "Debug" or "Release"
{
	const std::string build_configuration =
#if defined(NDEBUG) // i.e., release
		"Release";
#else
		"Debug";
#endif

	const auto Configuration = path.filename();
	const auto path_parent_path = path.parent_path();
	const auto x64 = path_parent_path.filename();
	return (Configuration == build_configuration) && (x64 == "x64");
}

static bool is_install_unittests(const fs::path& path)
{
	const auto unittests = path.filename();
	const auto path_parent_path = path.parent_path();
	const auto install = path_parent_path.filename();
	return (unittests == "unittests") && (install == "install");
}
static bool is_install_tests(const fs::path& path)
{
	const auto tests = path.filename();
	const auto path_parent_path = path.parent_path();
	const auto install = path_parent_path.filename();
	return (tests == "tests") && (install == "install");
}

static fs::path buildSchemaDir()
{
	const auto cwd = fs::current_path();

	const sys::OS os;
	const auto exec = fs::path(os.getCurrentExecutable());
	const auto argv0 = exec.filename();
	if (argv0 == "Test.exe")
	{
		// Running GTest unit-tests in Visual Studio on Windows
		if (is_x64_Configuration(cwd))
		{
			const auto root_path = cwd.parent_path().parent_path();
			return root_path / "six" / "modules" / "c++" / "six.sidd" / "conf" / "schema";
		}
	}

	if (argv0 == "unittests.exe")
	{
		// stand-alone unittest executable on Windows (ends in .EXE)
		const auto parent_path = exec.parent_path();
		if (is_x64_Configuration(parent_path))
		{
			const auto parent_path_ = parent_path.parent_path().parent_path();
			return parent_path_ / "dev" / "tests" / "images";
		}
	}

	// stand-alone unit-test on Linux
	const auto exec_dir = exec.parent_path();
	if (is_install_unittests(exec_dir))
	{
		const auto install = exec_dir.parent_path();
		return install / "unittests" / "data";
	}
	if (is_install_tests(exec_dir))
	{
		const auto install = exec_dir.parent_path();
		return install / "unittests" / "data";
	}

	if (argv0 == "unittests")
	{
		// stand-alone unittest executable on Linux
		const auto bin = exec.parent_path();
		if (bin.filename() == "bin")
		{
			const auto unittests = bin.parent_path();
			return unittests / "unittests" / "data";
		}
	}

	//fprintf(stderr, "cwd = %s\n", cwd.c_str());
	//fprintf(stderr, "exec = %s\n", exec.c_str());

	return cwd;
}

struct sidd_test_read_sidd_legend : public ::testing::Test {
    sidd_test_read_sidd_legend() {
        // initialization code here
        //const std::string SIX_SCHEMA_PATH = R"(C:\Users\jdsmith\source\repos\six\six\modules\c++\six.sidd\conf\schema)";
		const auto SIX_SCHEMA_PATH = buildSchemaDir();
		sys::OS().setEnv("SIX_SCHEMA_PATH", SIX_SCHEMA_PATH.string(), true /*overwrite*/);
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
