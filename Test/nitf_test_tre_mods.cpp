#include "pch.h"

#include <string>

#include <std/filesystem>
#include <sys/OS.h>

#include "nitf_Test.h"

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

static fs::path buildDir(const fs::path& path)
{
	const auto cwd = fs::current_path();

	const sys::OS os;
	const auto exec = fs::path(os.getCurrentExecutable());
	const auto argv0 = exec.filename();
	if (argv0 == "testhost.exe")
	{
		// Running Visual Studio unit-tests on Windows
		if (is_x64_Configuration(cwd))
		{
			return cwd / path;
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

static fs::path buildPluginsDir()
{
	return buildDir(fs::path("share") / "nitf" / "plugins");
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nitf_test_tre_mods) {
public:
	nitf_test_tre_mods()
	{
		// initialization code here
		//const std::string NITF_PLUGIN_PATH = R"(C:\Users\jdsmith\source\repos\nitro\x64\Debug\share\nitf\plugins)";
		const std::string putenv_ = "NITF_PLUGIN_PATH=" + buildPluginsDir().string();
		_putenv(putenv_.c_str());
	}

	~nitf_test_tre_mods() = default;
	nitf_test_tre_mods(const nitf_test_tre_mods&) = delete;
	nitf_test_tre_mods& operator=(const nitf_test_tre_mods&) = delete;

#define TEST_CASE(X) TEST_METHOD(X)
#include "nitf/unittests/test_tre_mods.c"

};