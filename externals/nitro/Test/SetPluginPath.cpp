#include "pch.h"

#include <string>

#include <import/sys.h>
#include <sys/Filesystem.h>

#include "nitf_Test.h"

namespace fs = std::filesystem;

static std::string Configuration() // "Configuration" is typically "Debug" or "Release"
{
#if defined(NDEBUG) // i.e., release
	return "Release";
#else
	return "Debug";
#endif
}

static std::string Platform()
{
#ifdef _WIN64 
	return "x64";
#else
	return "Win32";
#endif
}

// https://stackoverflow.com/questions/13794130/visual-studio-how-to-check-used-c-platform-toolset-programmatically
static std::string PlatformToolset()
{
	// https://docs.microsoft.com/en-us/cpp/build/how-to-modify-the-target-framework-and-platform-toolset?view=msvc-160
#if _MSC_FULL_VER >= 190000000
	return "v142";
#else
#error "Don't know $(PlatformToolset) value.'"
#endif
}

static bool is_x64_Configuration(const fs::path& path) // "Configuration" is typically "Debug" or "Release"
{
	const std::string build_configuration = Configuration();
	const auto Configuration = path.filename();
	const auto path_parent_path = path.parent_path();
	const auto x64 = path_parent_path.filename();
	return (Configuration == build_configuration) && (x64 == Platform());
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
	if (argv0 == "Test.exe")
	{
		// Running GTest unit-tests in Visual Studio on Windows
		if (is_x64_Configuration(cwd))
		{
			const auto root = cwd.parent_path().parent_path();
			const auto install = "install-" + Configuration() + "-" + Platform() + "." + PlatformToolset();
			return root / install / path;
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
std::string nitf::Test::buildPluginsDir()
{
	return ::buildPluginsDir().string();
}