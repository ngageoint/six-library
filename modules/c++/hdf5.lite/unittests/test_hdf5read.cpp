/* =========================================================================
 * This file is part of hdf5.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hdf5.lite-c++ is free software; you can redistribute it and/or modify
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

#include <string>
#include <std/filesystem>

#include <TestCase.h>

#include "sys/FileFinder.h"

#include "hdf5/lite/Read.h"

static std::filesystem::path findRootDirectory(const std::filesystem::path& p)
{
    // specific to CODA-OSS
    const auto isRoot = [](const std::filesystem::path& p) { return is_regular_file(p / "coda-oss-lite.sln") &&
        is_regular_file(p / "LICENSE") && is_regular_file(p / "README.md") && is_regular_file(p / "CMakeLists.txt"); };
    return sys::test::findRootDirectory(p, "coda-oss", isRoot);
}
inline std::filesystem::path findRoot()
{
    return findRootDirectory(std::filesystem::current_path());
}

TEST_CASE(test_hdf5Read)
{
    static const std::string file = "example.h5";
    const auto unittests = findRoot() / "modules" / "c++" / "hdf5.lite" / "unittests";
    const auto path = unittests / file;

    // https://www.mathworks.com/help/matlab/ref/h5read.html
    std::vector<double> data;
    const auto rc = hdf5::lite::readFile(path, "/g4/lat", data);
    TEST_ASSERT_EQ(rc.area(), 19);
    TEST_ASSERT_ALMOST_EQ(data[0], -90.0);
    TEST_ASSERT_ALMOST_EQ(data[0], -data[18]);
}

TEST_CASE(test_hdf5Read_IOException)
{
    static const std::filesystem::path path = "does not exist . h5";
    try
    {
        std::vector<double> data;
        hdf5::lite::readFile(path, "/g4/lat", data);
        TEST_FAIL;
    }
    catch (const except::IOException&)
    {
        TEST_SUCCESS;
    }
}

TEST_MAIN(
    TEST_CHECK(test_hdf5Read);
    TEST_CHECK(test_hdf5Read_IOException);
)
