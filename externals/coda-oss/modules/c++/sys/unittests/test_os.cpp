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

#include <fstream>
#include <sstream>

#include <sys/OS.h>
#include <sys/Path.h>
#include <sys/Filesystem.h>
#include "TestCase.h"

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
    testFsExtension_<coda_oss::filesystem::path>(testName);
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
    testFsOutput_<coda_oss::filesystem::path>(testName);
    #if CODA_OSS_lib_filesystem
    testFsOutput_<std::filesystem::path>(testName);
    #endif
}

}

int main(int, char**)
{
    TEST_CHECK(testRecursiveRemove);
    TEST_CHECK(testForcefulMove);
    TEST_CHECK(testEnvVariables);
    TEST_CHECK(testFsExtension);
    TEST_CHECK(testFsOutput);
    return 0;
}

