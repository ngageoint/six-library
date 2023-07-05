/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/UnitTests.hpp"

#include <assert.h>

#include <stdexcept>
#include <std/filesystem>
#include <std/optional>
#include <import/sys.h>
#include <sys/FileFinder.h>

namespace fs = std::filesystem;

static const sys::OS os;
static inline std::string Configuration() // "Configuration" is typically "Debug" or "Release"
{
	return os.getSpecialEnv("Configuration");
}
static inline std::string Platform()
{
	return os.getSpecialEnv("Platform"); // e.g., "x64" on Windows
}

static const fs::path& getCurrentExecutable()
{
	static const auto exec = fs::absolute(os.getCurrentExecutable());
	return exec;
}
static fs::path current_path()
{
	// the current working directory can change while a program is running
	return fs::absolute(fs::current_path());
}

static inline std::string PlatformToolset()
{
	return os.getSpecialEnv("PlatformToolset"); // e.g.,  "v143" on Windows
}

// Depending on the unittest we're running, how we're running it and what platform we're on, there could
//  be quite a varietry of paths:
// * Windows vs. Linux; Debug or Release (usually Windows-only)
// * Visual Studio unittest, CMake or WAF
// * As the "root" project or "externals" in another project (e.g., SIX)
//
// Furthermore, sample input files (e.g.) don't typically get built/installed, while
// TREs (dynamically loaded) are; so we need paths to both source code
// and install locations.

inline bool isRoot(const std::filesystem::path& p)
{
	// We typically have these files in any of our "root" directories, regardless of the proejct
	return is_regular_file(p / "LICENSE") && is_regular_file(p / "README.md") && is_regular_file(p / "CMakeLists.txt");
}
inline bool isNitroRoot(const std::filesystem::path& p)
{
	return is_regular_file(p / "nitro.sln") && isRoot(p); 	// specific to NITRO
}

// Build/install directories are usually here
static fs::path find_GIT_root()
{
	const auto is_GIT_root = [](const std::filesystem::path& p) { return is_directory(p / ".git") && isRoot(p); };
	try
	{
		return sys::test::findRootDirectory(getCurrentExecutable(), "" /*rootName*/, is_GIT_root);
	}
	catch (const std::invalid_argument&) {}

	return sys::test::findRootDirectory(current_path(), "" /*rootName*/, is_GIT_root);
}

// Sample files are usually here.
// This may be the same as find_GIT_root() if this code isn't in "externals"
static fs::path find_NITRO_root()
{
	try
	{
		return sys::test::findRootDirectory(getCurrentExecutable(), "nitro", isNitroRoot);
	}
	catch (const std::invalid_argument&) {}

	return sys::test::findRootDirectory(current_path(), "nitro", isNitroRoot);
}

static fs::path findRoot(fs::path& exec_root, fs::path& cwd_root)
{
	cwd_root.clear();
	try
	{
		exec_root = sys::test::findRootDirectory(getCurrentExecutable(), "" /*rootName*/, isRoot);
		return exec_root;
	}
	catch (const std::invalid_argument&) { }

	// CWD can change while the program is running
	cwd_root = sys::test::findRootDirectory(current_path(), "" /*rootName*/, isRoot);
	return cwd_root;
}
static fs::path findRoot()
{
	fs::path exec_root, cwd_root;
	return findRoot(exec_root, cwd_root);
}

static fs::path make_waf_install(const fs::path& p)
{
	// just "install" on Linux; "install-Debug-x64.v142" on Windows
#ifdef _WIN32
	const auto configuration_and_platform = Configuration() + "-" + Platform() + "." + PlatformToolset();
	return p / ("install-" + configuration_and_platform);
#else
	// Linux
	return p / "install";
#endif
}

static bool is_cmake_build()
{
	static const auto retval = sys::test::isCMakeBuild(getCurrentExecutable());
	return retval;
}

static fs::path buildDir(const fs::path& relativePath)
{
	std::clog << "getCurrentExecutable(): " << getCurrentExecutable() << '\n';
	std::clog << "current_path(): " << current_path() << '\n';

	static const auto& exec = getCurrentExecutable();
	static const auto exec_filename = exec.filename();

	if (exec_filename == "testhost.exe")
	{
		// Running in Visual Studio on Windows
		return current_path() / relativePath;
	}

	const auto p = is_cmake_build() ? sys::test::findCMakeBuildRoot(exec) : make_waf_install(findRoot());
	return p / relativePath;
}

std::string buildPluginsDir_(const std::string& dir, const std::filesystem::path& installDir)
{
	// Developers might not set things up for "cmake --install ."
	const auto modules_c_dir = std::filesystem::path("modules") / "c" / dir;
	auto plugins = installDir / modules_c_dir / Configuration();
	std::clog << "plugins: " << plugins << '\n';
	if (is_directory(plugins))
	{
		return plugins.string();
	}
	plugins = installDir / modules_c_dir;
	std::clog << "plugins: " << plugins << '\n';
	if (is_directory(plugins))
	{
		return plugins.string();
	}

	static const auto exec = getCurrentExecutable();
	auto buildRoot = sys::test::findCMakeBuildRoot(exec);
	plugins = buildRoot / modules_c_dir;
	std::clog << "plugins: " << plugins << '\n';
	if (is_directory(plugins))
	{
		return plugins.string();
	}

	buildRoot = buildRoot / "externals" / "nitro";
	plugins = buildRoot / modules_c_dir;
	std::clog << "plugins: " << plugins << '\n';
	if (is_directory(plugins))
	{
		return plugins.string();
	}

	throw std::logic_error("Can't find 'plugins' directory: " + plugins.string());
}
std::string nitf::Test::buildPluginsDir(const std::string& dir)
{
	auto installDir = buildDir("");
	auto plugins = installDir / "share" / "nitf" / "plugins";
	if (!is_directory(plugins))
	{
		// Developers might not set things up for "cmake --install ."
		return buildPluginsDir_(dir, installDir);
	}
	return plugins.string();
}

fs::path nitf::Test::buildFileDir(const fs::path& relativePath)
{
	const auto root = find_GIT_root();
	return root / relativePath;
}

fs::path nitf::Test::findInputFile(const fs::path& inputFile)
{
	const auto root = find_NITRO_root();

	auto p = root / inputFile;
	if (is_regular_file(p))
	{
		return p;
	}

	p = sys::findFirstFile(root, inputFile);
	return p / inputFile;
}

fs::path nitf::Test::findInputFile(const fs::path& modulePath, const fs::path& moduleFile)
{
	return sys::test::findGITModuleFile("nitro", modulePath, moduleFile);
}

static std::filesystem::path getNitfPluginPath(const std::string& pluginName)
{
	std::filesystem::path p;
	try
	{
		p = nitf::Test::buildPluginsDir();
	}
	catch (const std::invalid_argument&)
	{
		p = getCurrentExecutable();
	}
	auto plugin = p / pluginName;
	if (!is_regular_file(plugin))
	{
		p = sys::findFirstFile(p, pluginName);
		plugin = p / pluginName;
		if (!is_regular_file(plugin))
		{
			throw std::logic_error("Can't find plugin: " + plugin.string());
		}
	}
	return p;
}

static std::string buildPluginName(const std::string& base)
{
#ifdef _WIN32
	return base + ".DLL";
#else
	// Note that these do NOT have the typical "lib" prefix;
	// i.e., file is "ENGRDA.so" instead of "libENGRDA.so"
	return base + ".so";
#endif
}

void nitf::Test::setNitfPluginPath()
{
	// The name of the plugin we know exists and will always be built, see test_load_plugins
	static const auto p = getNitfPluginPath(buildPluginName("ENGRDA"));
	sys::OS().setEnv("NITF_PLUGIN_PATH", p.string(), true /*overwrite*/);	
}

void nitf::Test::j2kSetNitfPluginPath()
{
	static const auto p = getNitfPluginPath(buildPluginName("J2KCompress"));
	sys::OS().setEnv("NITF_PLUGIN_PATH", p.string(), true /*overwrite*/);
}