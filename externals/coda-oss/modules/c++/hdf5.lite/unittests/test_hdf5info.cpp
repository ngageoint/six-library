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

#include "hdf5/lite/Info.h"

static std::filesystem::path find_unittest_file(const std::filesystem::path& name)
{
    static const auto unittests = std::filesystem::path("modules") / "c++" / "hdf5.lite" / "unittests";
    return sys::test::findGITModuleFile("coda-oss", unittests, name);
}
TEST_CASE(test_hdf5Info_IOException)
{
    static const std::filesystem::path path = "does not exist . h5";
     try
    {
        // https://www.mathworks.com/help/matlab/ref/h5info.html
        const auto info = hdf5::lite::fileInfo(path);
        TEST_FAIL;
    }
    catch (const except::IOException&)
    {
        TEST_SUCCESS;
    }
}


TEST_CASE(test_hdf5FileInfo)
{
    static const auto path = find_unittest_file("example.h5");

    // https://www.mathworks.com/help/matlab/ref/h5info.html
    const auto info = hdf5::lite::fileInfo(path);
    TEST_ASSERT_EQ(path.string(), info.filename);
    TEST_ASSERT_EQ("/", info.name);
    TEST_ASSERT_EQ(info.groups.size(), 4);
    TEST_ASSERT_TRUE(info.datasets.empty());
    TEST_ASSERT_TRUE(info.datatypes.empty());
    //TEST_ASSERT_TRUE(info.links.empty());
    //TEST_ASSERT_EQ(info.attributes.size(), 2);
}

TEST_CASE(test_hdf5GroupInfo)
{
    static const auto path = find_unittest_file("example.h5");

    // https://www.mathworks.com/help/matlab/ref/h5info.html
    const auto info = hdf5::lite::groupInfo(path, "/g4");

    TEST_ASSERT_EQ(path.string(), info.filename);
    TEST_ASSERT_EQ("/g4", info.name);
    TEST_ASSERT_TRUE(info.groups.empty());
    TEST_ASSERT_EQ(info.datasets.size(), 4);
    TEST_ASSERT_TRUE(info.datatypes.empty());
    //TEST_ASSERT_TRUE(info.links.empty());
    //TEST_ASSERT_TRUE(info.attributes.empty());
}

TEST_CASE(test_hdf5DatasetInfo)
{
    static const auto path = find_unittest_file("example.h5");

    // https://www.mathworks.com/help/matlab/ref/h5info.html
    const auto info = hdf5::lite::datasetInfo(path, "/g4/time");

    TEST_ASSERT_EQ(path.string(), info.filename);
    TEST_ASSERT_EQ("time", info.name);
    TEST_ASSERT(info.datatype.h5Class == hdf5::lite::Class::Float);
    //TEST_ASSERT(info.dataspace == hdf5::lite::Class::Float);
    // TEST_ASSERT(info.chunkSize == hdf5::lite::Class::Float);
    // TEST_ASSERT(info.fillValue == hdf5::lite::Class::Float);
    //TEST_ASSERT_TRUE(info.filters.empty());
    //TEST_ASSERT_EQ(info.attributes.size(), 2);
}

TEST_MAIN(
    TEST_CHECK(test_hdf5Info_IOException);

    TEST_CHECK(test_hdf5FileInfo);
    TEST_CHECK(test_hdf5GroupInfo);
    TEST_CHECK(test_hdf5DatasetInfo);
)
