/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <assert.h>

#include <fstream>
#include <sstream>
#include <numeric> // std::accumulate
#include <string>

#include <std/filesystem>

#include <sys/OS.h>
#include <sys/Path.h>
#include <sys/Filesystem.h>
#include <sys/Backtrace.h>
#include <sys/Dbg.h>
#include <sys/DateTime.h>
#include "TestCase.h"

namespace fs = sys::Filesystem;

namespace
{
void createFile(const std::string& pathname)
{
    std::ofstream oss(pathname.c_str());
    oss.write(pathname.c_str(), pathname.length());
    oss.close();
}

TEST_CASE(testRecursiveRemove)
{
    // This assumes the user has write permissions in their current directory
    const sys::OS os;
    const sys::Path subdir1("subdir1");
    TEST_ASSERT( os.makeDirectory(subdir1) );
    createFile(subdir1.join("tempFile1"));
    createFile(subdir1.join("tempFile2"));

    const sys::Path subdir2(subdir1.join("subdir2"));
    TEST_ASSERT( os.makeDirectory(subdir2) );

    const sys::Path subdir3(subdir2.join("subdir3"));
    TEST_ASSERT( os.makeDirectory(subdir3) );
    createFile(subdir3.join("tempFile3"));
    createFile(subdir3.join("tempFile4"));
    createFile(subdir3.join("tempFile5"));

    // Try to recursively remove from the top level
    try
    {
        os.remove(subdir1);
    }
    catch (...)
    {
        TEST_ASSERT(false);
    }
    TEST_ASSERT( !os.exists(subdir1) );
}

TEST_CASE(testForcefulMove)
{
    // This assumes the user has write permissions in their current directory
    const sys::OS os;
    const sys::Path subdir1("subdir1");
    TEST_ASSERT( os.makeDirectory(subdir1) );
    createFile(subdir1.join("tempFile1"));
    createFile(subdir1.join("tempFile2"));
    createFile(subdir1.join("tempFile3"));

    const sys::Path subdir2(subdir1.join("subdir2"));
    TEST_ASSERT( os.makeDirectory(subdir2) );

    // regular move
    if (!os.move(subdir1.join("tempFile2"), subdir2.join("tempFile2")))
    {
        TEST_ASSERT(false);
    }

    // forceful move
    if (!os.move(subdir1.join("tempFile3"), subdir1.join("tempFile1")))
    {
        TEST_ASSERT(false);
    }

    // Try to recursively remove from the top level
    try
    {
        os.remove(subdir1);
    }
    catch (...)
    {
        TEST_ASSERT(false);
    }
    TEST_ASSERT( !os.exists(subdir1) );
}

TEST_CASE(testEnvVariables)
{
    sys::OS os;
    const std::string testvar = "TESTVARIABLE";
    const std::string testvalue = "TESTVALUE";
    const std::string testvalue2 = "TESTVALUE2";

    // Start by clearing the environment variable, if set.
    os.unsetEnv(testvar);
    TEST_ASSERT_FALSE(os.isEnvSet(testvar));

    // Check getEnv throws an sys::SystemException exception when trying unset var

    TEST_SPECIFIC_EXCEPTION(os.getEnv(testvar),sys::SystemException);

    // Test getEnvIfSet doesn't update value and returns false on unset var.
    std::string candidatevalue="Unset";

    TEST_ASSERT_FALSE(os.getEnvIfSet(testvar, candidatevalue));
    TEST_ASSERT_EQ(candidatevalue,"Unset");

    // Set the environment variable
    os.setEnv(testvar, testvalue, true);

    TEST_ASSERT(os.isEnvSet(testvar));

    TEST_ASSERT(os.getEnvIfSet(testvar, candidatevalue));
    TEST_ASSERT_EQ(candidatevalue,testvalue);
    std::string getEnvVar = os.getEnv(testvar);
    TEST_ASSERT_EQ(getEnvVar,testvalue);

    // Set the environment variable without overwrite. (Should not update).
    os.setEnv(testvar, testvalue2,  false);

    TEST_ASSERT(os.getEnvIfSet(testvar, candidatevalue));
    TEST_ASSERT_EQ(candidatevalue,testvalue);
    getEnvVar = os.getEnv(testvar);
    TEST_ASSERT_EQ(getEnvVar,testvalue);

    // Finally unset the variable again.
    os.unsetEnv(testvar);

    TEST_ASSERT_FALSE(os.isEnvSet(testvar));
}

TEST_CASE(testSplitEnv)
{
    sys::OS os;

    // PATH is usually set to multiple directories on both Windows and *nix
    const std::string pathEnvVar = "PATH";
    std::vector<std::string> paths;
    bool result = os.splitEnv(pathEnvVar, paths);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_GREATER(paths.size(), 0);
    for (const auto& path : paths)
    {
        TEST_ASSERT_TRUE(sys::Filesystem::exists(path));
    }

    // create an environemnt variable with a known bogus path
    const auto bogusValue =  paths[0] + sys::Path::separator() + "this does not exist";
    paths.clear();
    const std::string bogusEnvVar = "CODA_OSS_TEST_PATH";
    std::string value;
    TEST_ASSERT_FALSE(os.getEnvIfSet(bogusEnvVar, value));
    os.setEnv(bogusEnvVar, bogusValue, false /*overwrite*/);
    result = os.splitEnv(bogusEnvVar, paths);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQ(paths.size(), 1);

    // PATHs are directories, not files
    paths.clear();
    result = os.splitEnv(pathEnvVar, paths, sys::Filesystem::FileType::Directory);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_GREATER(paths.size(), 0);
    paths.clear();
    result = os.splitEnv(pathEnvVar, paths, sys::Filesystem::FileType::Regular);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(paths.empty());

    const std::string notFoundEnvVar = "CODA_OSS_SOME_VAR_THAT_WE_KNOW_WONT_BE_SET";
    paths.clear();
    result = os.splitEnv(notFoundEnvVar, paths);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(paths.empty());
}

template <typename TPath>
static void testFsExtension_(const std::string& testName)
{
    using fs_path = TPath;

    // https://en.cppreference.com/w/cpp/filesystem/path/extension

    // "If the pathname is either . or .., ... then empty path is returned."
    const fs_path dot(".");
    TEST_ASSERT_EQ("", dot.extension());
    const fs_path dotdot("..");
    TEST_ASSERT_EQ("", dotdot.extension());

    // "If the first character in the filename is a period, that period is ignored
    // (a filename like '.profile' is not treated as an extension)"
    fs_path dotprofile("/path/to/.profile");
    TEST_ASSERT_EQ("", dotprofile.extension());
    dotprofile = ".profile";
    TEST_ASSERT_EQ("", dotprofile.extension());
    dotprofile = "/path/to/.profile.user";
    TEST_ASSERT_EQ(".user", dotprofile.extension());
    dotprofile = "/path.to/.profile.user";
    TEST_ASSERT_EQ(".user", dotprofile.extension());
    dotprofile = ".profile.user";
    TEST_ASSERT_EQ(".user", dotprofile.extension());

    fs_path filedottext("/path/to/file.txt");
    TEST_ASSERT_EQ(".txt", filedottext.extension());
    filedottext = "file.txt";
    TEST_ASSERT_EQ(".txt", filedottext.extension());

    // "If ... filename() does not contain the . character, then empty path is returned."
    filedottext = "/path/to/file";
    TEST_ASSERT_EQ("", filedottext.extension());
    filedottext = "file";
    TEST_ASSERT_EQ("", filedottext.extension());
    filedottext = "/path.to/file";
    TEST_ASSERT_EQ("", filedottext.extension());
}
TEST_CASE(testFsExtension)
{
    testFsExtension_<sys::Filesystem::path>(testName);
    testFsExtension_<std::filesystem::path>(testName);
    #if CODA_OSS_lib_filesystem
    testFsExtension_<std::filesystem::path>(testName);
    #endif
}

template <typename TPath>
static void testFsOutput_(const std::string& testName)
{
    using fs_path = TPath;

    const fs_path path("/path/to/file.txt");
    const std::string expected = "\"" + path.string() + "\"";

    std::stringstream ss;
    ss << path;
    const auto actual = ss.str();
    TEST_ASSERT_EQ(expected, actual);
}
TEST_CASE(testFsOutput)
{
    testFsOutput_<sys::Filesystem::path>(testName);
    testFsOutput_<std::filesystem::path>(testName);
    #if CODA_OSS_lib_filesystem
    testFsOutput_<std::filesystem::path>(testName);
    #endif
}

static std::string f(bool& supported, std::vector<std::string>& frames)
{
    return sys::getBacktrace(supported, frames);
}
static std::string g(bool& supported, std::vector<std::string>& frames)
{
    return f(supported, frames);
}
static std::string h(bool& supported, std::vector<std::string>& frames)
{
    return g(supported, frames);
}
TEST_CASE(testBacktrace)
{
    bool supported;
    std::vector<std::string> frames;
    const auto result = h(supported, frames);
    TEST_ASSERT_TRUE(!result.empty());
    const auto failed_pos = result.find(" failed.");
    TEST_ASSERT_EQ(failed_pos, std::string::npos);


    size_t frames_size = 0;
    auto version_sys_backtrace_ = version::sys::backtrace; // "Conditional expression is constant"
    if (version_sys_backtrace_ >= 20210216L)
    {
        TEST_ASSERT_TRUE(supported);

        #if _WIN32
        constexpr auto frames_size_RELEASE = 2;
        constexpr auto frames_size_DEBUG = 14;
        #elif defined(__GNUC__)
        constexpr auto frames_size_RELEASE = 6;
        constexpr auto frames_size_DEBUG = 10;
        #else
        #error "CODA_OSS_sys_Backtrace inconsistency."
        #endif
        frames_size = sys::debug_build ? frames_size_DEBUG : frames_size_RELEASE;
    }
    else
    {
        TEST_ASSERT_FALSE(supported);
    }
    TEST_ASSERT_EQ(frames.size(), frames_size);

    const auto msg = std::accumulate(frames.begin(), frames.end(), std::string());
    if (supported)
    {
        TEST_ASSERT_EQ(result, msg);
    }
    else
    {
        TEST_ASSERT_TRUE(msg.empty());
    }
}

TEST_CASE(testSpecialEnvVars)
{
    const sys::OS os;
    const auto argv0 = os.getSpecialEnv("ARGV0");
    TEST_ASSERT_FALSE(argv0.empty());
    auto result = os.getSpecialEnv("0"); // i.e., ${0)
    TEST_ASSERT_FALSE(result.empty());
    TEST_ASSERT_EQ(result, argv0);
    const fs::path fsresult(result);
    const fs::path this_file(__FILE__);
    TEST_ASSERT_EQ(fsresult.stem(), this_file.stem());

    const auto pid = os.getSpecialEnv("PID");
    TEST_ASSERT_FALSE(pid.empty());
    result = os.getSpecialEnv("$"); // i.e., ${$}
    TEST_ASSERT_FALSE(result.empty());
    TEST_ASSERT_EQ(result, pid);
    const auto strPid = std::to_string(os.getProcessId());
    TEST_ASSERT_EQ(result, strPid);

    result = os.getSpecialEnv("PWD");
    TEST_ASSERT_FALSE(result.empty());

    result = os.getSpecialEnv("USER");
    TEST_ASSERT_FALSE(result.empty());
    const auto username = os.getSpecialEnv("USERNAME");
    TEST_ASSERT_FALSE(result.empty());
    TEST_ASSERT_EQ(username, result);

    result = os.getSpecialEnv("HOSTNAME");
    TEST_ASSERT_FALSE(result.empty());

    result = os.getSpecialEnv("SECONDS");
    TEST_ASSERT_FALSE(result.empty());
    const auto seconds = std::stoll(result);
    TEST_ASSERT_GREATER_EQ(seconds, 0);

    const auto epochSeconds = sys::DateTime::getEpochSeconds();
    result = os.getSpecialEnv("EPOCHSECONDS");
    TEST_ASSERT_FALSE(result.empty());
    const auto resultEpochSeconds = std::stoll(result);
    TEST_ASSERT_GREATER_EQ(resultEpochSeconds, epochSeconds);

    result = os.getSpecialEnv("Configuration");
    TEST_ASSERT_FALSE(result.empty());
    result = os.getSpecialEnv("Platform");
    TEST_ASSERT_FALSE(result.empty());
}

}

int main(int, char** argv)
{
    sys::AbstractOS::setArgvPathname(argv[0]);

    TEST_CHECK(testRecursiveRemove);
    TEST_CHECK(testForcefulMove);
    TEST_CHECK(testEnvVariables);
    TEST_CHECK(testSplitEnv);
    TEST_CHECK(testFsExtension);
    TEST_CHECK(testFsOutput);
    TEST_CHECK(testBacktrace);
    TEST_CHECK(testSpecialEnvVars);

    return 0;
}

