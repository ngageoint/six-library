/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#include <sys/Path.h>
#include <sys/Filesystem.h>
#include "TestCase.h"

namespace fs = sys::Filesystem;

namespace
{
TEST_CASE(testPathMerge)
{
    const sys::OS os;

    // PATH is usually set to multiple directories on both Windows and *nix
    std::vector<std::string> paths;
    const auto splitResult = os.splitEnv("PATH", paths);
    TEST_ASSERT_TRUE(splitResult);
    TEST_ASSERT_GREATER(paths.size(), 0);

    std::string path;
    for (const auto& p : paths)
    {
        if (sys::Filesystem::is_directory(p))
        {
            path = p;
            break;
        }
    }
    TEST_ASSERT_TRUE(sys::Filesystem::is_directory(path));
    // add trailing '/'
    if (!str::endsWith(path, sys::Path::delimiter()))
    {
        path += sys::Path::delimiter();
    }

    bool isAbsolute;
    auto components = sys::Path::separate(path, isAbsolute);
    TEST_ASSERT_GREATER(components.size(), 0);
    auto result = sys::Path::merge(components, isAbsolute);
    TEST_ASSERT_EQ(result, path);
    TEST_ASSERT_TRUE(sys::Filesystem::is_directory(result));

    #if _WIN32
    path = R"(C:\dir\file.txt)";
    #else
    path = R"(/dir1/dir2/file.txt)";
    #endif
    components = sys::Path::separate(path, isAbsolute);
    TEST_ASSERT_EQ(components.size(), 3);
    result = sys::Path::merge(components, isAbsolute);
    TEST_ASSERT_EQ(result, path);
}

TEST_CASE(testExpandEnvTilde)
{
    auto path = sys::Path::expandEnvironmentVariables("~");
    TEST_ASSERT_TRUE(fs::is_directory(path));

    path = sys::Path::expandEnvironmentVariables("~", sys::Filesystem::FileType::Directory);
    TEST_ASSERT_TRUE(fs::is_directory(path));

    path = sys::Path::expandEnvironmentVariables("~", sys::Filesystem::FileType::Regular);
    TEST_ASSERT_TRUE(path.empty());
}

TEST_CASE(testExpandEnvTildePath)
{
    sys::OS os;
    const std::vector<std::string> exts{"NTUSER.DAT", ".login", ".cshrc", ".bashrc"};
    os.prependEnv("exts", exts, true /*overwrite*/);

    const auto path = sys::Path::expandEnvironmentVariables("~/$(exts)", sys::Filesystem::FileType::Regular);
    TEST_ASSERT_TRUE(sys::Filesystem::is_regular_file(path));
}

TEST_CASE(testExpandEnv)
{
    sys::OS os;
    os.setEnv("CODA_OSS_test", "CODA_OSS_test", true /*overwrite*/);

    const auto test = sys::Path::expandEnvironmentVariables("$CODA_OSS_test", false);
    TEST_ASSERT_FALSE(test.empty());

    #if _WIN32  // %FOO% only on Windows
    const auto win32_test = sys::Path::expandEnvironmentVariables("%CODA_OSS_test%", false);
    TEST_ASSERT_EQ(win32_test, test);
    #endif

    auto result = sys::Path::expandEnvironmentVariables("$(CODA_OSS_test)", false);
    TEST_ASSERT_EQ(result, test);

    result = sys::Path::expandEnvironmentVariables("${CODA_OSS_test}", false);
    TEST_ASSERT_EQ(result, test);

    result = sys::Path::expandEnvironmentVariables("foo${CODA_OSS_test}", false);
    TEST_ASSERT_EQ(result, "foo" + test);

    result = sys::Path::expandEnvironmentVariables("${CODA_OSS_test}foo", false);
    TEST_ASSERT_EQ(result, test + "foo");

    result = sys::Path::expandEnvironmentVariables("foo${CODA_OSS_test}bar", false);
    TEST_ASSERT_EQ(result, "foo" + test + "bar");

    result = sys::Path::expandEnvironmentVariables("foo$CODA_OSS_test-bar", false);
    TEST_ASSERT_EQ(result, "foo" + test + "-bar");

    result = sys::Path::expandEnvironmentVariables("foo$CODA_OSS_test(bar)", false);
    TEST_ASSERT_EQ(result, "foo" + test + "(bar)");

    result = sys::Path::expandEnvironmentVariables("foo$CODA_OSS_test)bar(", false);
    TEST_ASSERT_EQ(result, "foo" + test + ")bar(");

    result = sys::Path::expandEnvironmentVariables("foo$(CODA_OSS_test)BAR)", false);
    TEST_ASSERT_EQ(result, "foo" + test + "BAR)");

    result = sys::Path::expandEnvironmentVariables("$CODA_OSS_test$CODA_OSS_test", false);
    TEST_ASSERT_EQ(result, test + test);
    #if _WIN32  // %FOO% only on Windows
    result = sys::Path::expandEnvironmentVariables("%CODA_OSS_test%%CODA_OSS_test%", false);
    TEST_ASSERT_EQ(result, test + test);
    #endif

    result = sys::Path::expandEnvironmentVariables("foo$CODA_OSS_test-bar$(CODA_OSS_test)BAR)", false);
    TEST_ASSERT_EQ(result, "foo" + test + "-bar" + test + "BAR)");
}

TEST_CASE(testExpandEnvPathExists)
{
    sys::OS os;
    #ifdef _WIN32
    const std::string does_not_exist(R"(Q:\Does\Not\Exist)");
    #else
    const std::string does_not_exist(R"(/does/not/existt)");
    #endif
    std::vector<std::string> values{does_not_exist};
    os.prependEnv("PATH", values, true /*overwrite*/);

    const auto path = sys::Path::expandEnvironmentVariables("$PATH");
    TEST_ASSERT_FALSE(path.empty());

    #if _WIN32  // %FOO% only on Windows
    const auto win32_path = sys::Path::expandEnvironmentVariables("%PATH%");
    TEST_ASSERT_EQ(win32_path, path);
    #endif

    const auto path2 = sys::Path::expandEnvironmentVariables("$(PATH)");
    TEST_ASSERT_EQ(path2, path);

    const auto path3 = sys::Path::expandEnvironmentVariables("${PATH}");
    TEST_ASSERT_EQ(path3, path);
}

TEST_CASE(testExpandEnvPathMultiple)
{
    sys::OS os;

    const std::vector<std::string> paths{"home", "opt", "var"};
    os.prependEnv("paths", paths, true /*overwrite*/);
    auto expanded_path = sys::Path::expandEnvironmentVariables("$(paths)", false /*checkIfExists*/);
    std::string home = "home";
    if (fs::is_directory(home) && !str::endsWith(home, sys::Path::delimiter()))
    {
        home += sys::Path::delimiter();
    }
    TEST_ASSERT_EQ(expanded_path, home);
    auto expanded_paths = sys::Path::expandedEnvironmentVariables("$(paths)");
    TEST_ASSERT_EQ(expanded_paths.size(), 3);

    const std::vector<std::string> apps{"apps"};
    os.prependEnv("apps", apps, true /*overwrite*/);
    const std::vector<std::string> app{"app_v1", "app_v2"};
    os.prependEnv("app", app, true /*overwrite*/);
    const std::vector<std::string> libs{"lib", "lib64"};
    os.prependEnv("libs", libs, true /*overwrite*/);
    const std::vector<std::string> exts{"libfoo.a", "libfoo.so", "foo.dll"};
    os.prependEnv("exts", exts, true /*overwrite*/);

    const std::string path_to_expand_root =
    #if _WIN32
    "C:";
    #else
    "/disk0";
    #endif
    const std::string path_to_expand = path_to_expand_root + "/$(paths)/$(apps)/$(app)/$(libs)/$(exts)";
    const std::vector<std::string> expected{path_to_expand_root, paths[0], apps[0], app[0], libs[0], exts[0]};
    auto expected_path = sys::Path::merge(expected, true /*isAbsolute*/);
    expanded_path = sys::Path::expandEnvironmentVariables(path_to_expand, false /*checkIfExists*/);
    TEST_ASSERT_EQ(expanded_path, expected_path);

    expanded_paths = sys::Path::expandedEnvironmentVariables(path_to_expand);
    TEST_ASSERT_EQ(expanded_paths.size(), paths.size() * apps.size() * app.size() * libs.size() * exts.size());
    const std::vector<std::string> expected_back{path_to_expand_root, paths.back(), apps.back(), app.back(), libs.back(), exts.back()};
    expected_path = sys::Path::merge(expected_back, true /*isAbsolute*/);
    TEST_ASSERT_EQ(expanded_paths.back(), expected_path);
}


TEST_CASE(testModifyVar)
{
    const sys::OS os;

    const auto argv0_t = sys::Path::expandEnvironmentVariables("${ARGV0@t}", false /*checkIfExists*/);
    TEST_ASSERT_FALSE(argv0_t.empty());

    const auto result = os.getSpecialEnv("0");  // i.e., ${0)
    TEST_ASSERT_FALSE(result.empty());
    const fs::path fsresult(result);
    const fs::path this_file(__FILE__);
    TEST_ASSERT_EQ(fsresult.stem(), this_file.stem());
    TEST_ASSERT_EQ(argv0_t, fsresult.filename());
}

static std::string modifyEnv(const std::string& envVar, char op)
{
  const auto strExpand = "${" + envVar + "@" + op + "}";
  return sys::Path::expandEnvironmentVariables(strExpand, false /*checkIfExists*/);
}
TEST_CASE(testModifyVar2)
{
    sys::OS os;

    /*
      #!/bin/csh -f

      # http://www.kitebird.com/csh-tcsh-book/tcsh.pdf
      # The word or words in a history reference can be edited, or "modified", by following it with one or more modifiers,
      # each preceded by a ':':
      #    h Remove a trailing pathname component, leaving the head.
      #    t Remove all leading pathname components, leaving the tail.
      #    r Remove a filename extension '.xxx', leaving the root name.
      #    e Remove all but the extension.

      set path=/dir1/dir2/file.txt
      echo "path=$path"
      echo ":h $path:h"", /dir1/dir2"
      echo ":t $path:t"", file.txt"
      echo ":r $path:r"", /dir1/dir2/file"
      echo ":e $path:e"", txt"

      echo ""
      set path=/dir1/dir2/
      echo "path=$path"
      echo ":h $path:h"", /dir1/dir2"
      echo ":t $path:t"
      echo ":r $path:r"", /dir1/dir2/"
      echo ":e $path:e"

      echo ""
      set path=/dir1/dir2
      echo "path=$path"
      echo ":h $path:h"", /dir1/dir2"
      echo ":t $path:t", "dir2"
      echo ":r $path:r"", /dir1/dir2"
      echo ":e $path:e"

      ------------------------------------------------------

      path=/dir1/dir2/file.txt
      :h /dir1/dir2, /dir1/dir2
      :t file.txt, file.txt
      :r /dir1/dir2/file, /dir1/dir2/file
      :e txt, txt

      path=/dir1/dir2/
      :h /dir1/dir2, /dir1/dir2
      :t 
      :r /dir1/dir2/, /dir1/dir2/
      :e 

      path=/dir1/dir2
      :h /dir1, /dir1/dir2
      :t dir2, dir2
      :r /dir1/dir2, /dir1/dir2
      :e 
    */

    constexpr auto path = "/dir1/dir2/file.txt";
    const std::string envVar = "CODA_OSS_test_path_path";
    os.setEnv(envVar, path, true /*overwrite*/);

    const auto h = modifyEnv(envVar, 'h');
    TEST_ASSERT_EQ(h, "/dir1/dir2");
    const auto t = modifyEnv(envVar, 't');
    TEST_ASSERT_EQ(t, "file.txt");
    const auto r = modifyEnv(envVar, 'r');
    // TEST_ASSERT_EQ(r, "/dir1/dir2/file"); // TODO: fix on Windows
    const auto e = modifyEnv(envVar, 'e');
    TEST_ASSERT_EQ(e, "txt");
    const auto s = modifyEnv(envVar, 's');
    TEST_ASSERT_EQ(s, "file");
}

}

int main(int, char**)
{
    TEST_CHECK(testPathMerge);
    TEST_CHECK(testExpandEnvTilde);
    TEST_CHECK(testExpandEnv);
    TEST_CHECK(testExpandEnvTildePath);
    TEST_CHECK(testExpandEnvPathExists);
    TEST_CHECK(testExpandEnvPathMultiple);
    TEST_CHECK(testModifyVar);
    TEST_CHECK(testModifyVar2);

    return 0;
}

