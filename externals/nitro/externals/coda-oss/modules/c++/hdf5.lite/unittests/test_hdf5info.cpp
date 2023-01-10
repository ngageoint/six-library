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
    /*
    info = struct with fields:
          Filename: '/mathworks/devel/bat/Bdoc22b/build/matlab/toolbox/matlab/demos/example.h5'
              Name: '/'
            Groups: [4x1 struct]
          Datasets: []
         Datatypes: []
             Links: []
        Attributes: [2x1 struct]
    */
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
    /*
    info = struct with fields:
      Filename: '/mathworks/devel/bat/Bdoc22b/build/matlab/toolbox/matlab/demos/example.h5'
          Name: '/g4'
        Groups: []
      Datasets: [4x1 struct]
     Datatypes: []
         Links: []
    Attributes: []
    */
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
    /*
    info = struct with fields:
      Filename: '/mathworks/devel/bat/Bdoc22b/build/matlab/toolbox/matlab/demos/example.h5'
          Name: 'time'
      Datatype: [1x1 struct]
     Dataspace: [1x1 struct]
     ChunkSize: 10
     FillValue: 0
       Filters: []
    Attributes: [2x1 struct]    
    */
    TEST_ASSERT_EQ(path.string(), info.filename);
    TEST_ASSERT_EQ("time", info.name);
    TEST_ASSERT(info.datatype.h5Class == hdf5::lite::Class::Float);
    //TEST_ASSERT(info.dataspace == hdf5::lite::Class::Float);
    //TEST_ASSERT_EQ(info.chunkSize, 10);
    //TEST_ASSERT_EQ(info.fillValue, 0);
    //TEST_ASSERT_TRUE(info.filters.empty());
    //TEST_ASSERT_EQ(info.attributes.size(), 2);
}

static void read_complex(const std::string& testName,
    const std::filesystem::path& path, const std::string& datasetPath)
{
    const auto i_info = hdf5::lite::datasetInfo(path, datasetPath + "/i");
    TEST_ASSERT(i_info.datatype.h5Class == hdf5::lite::Class::Float);

    const auto r_info = hdf5::lite::datasetInfo(path, datasetPath + "/r");
    TEST_ASSERT(r_info.datatype.h5Class == hdf5::lite::Class::Float);
}

TEST_CASE(test_hdf5Info_nested)
{
    /*
    Group '/' 
    Group '/1' 
        Group '/1/bar' 
            Group '/1/bar/cat' 
                Dataset 'i' 
                    Size:  10x1
                    MaxSize:  10x1
                    Datatype:   H5T_IEEE_F64LE (double)
                    ChunkSize:  []
                    Filters:  none
                    FillValue:  0.000000
    */

    // outer groups: 1, 2, 3
    // sub groups: bar, foo
    // sub-sub groups: cat, dog
    // data: i (float array), r (float array)
    static const auto path = find_unittest_file("123_barfoo_catdog_cx.h5");

    // https://www.mathworks.com/help/matlab/ref/h5info.html
    const auto info = hdf5::lite::fileInfo(path);
    TEST_ASSERT_EQ(path.string(), info.filename);
    TEST_ASSERT_EQ("/", info.name);
    TEST_ASSERT_TRUE(info.datasets.empty());
    TEST_ASSERT_TRUE(info.datatypes.empty());

    const std::vector<std::string> expectedOuterGroupNames{"1" , "2", "3"};
    TEST_ASSERT_EQ(info.groups.size(), expectedOuterGroupNames.size());
    for (size_t outer = 0; outer < info.groups.size(); outer++)
    {
        const auto groupName = info.groups[outer].name;
        TEST_ASSERT_EQ(groupName, expectedOuterGroupNames[outer]);

        const auto subGroupInfo = hdf5::lite::groupInfo(path, "/" + groupName);

        const std::vector<std::string> expectedSubGroupNames{"bar", "foo"};
        TEST_ASSERT_EQ(subGroupInfo.groups.size(), expectedSubGroupNames.size());
        for (size_t sub = 0; sub < subGroupInfo.groups.size(); sub++)
        {
            const auto subGroupName = subGroupInfo.groups[sub].name;
            TEST_ASSERT_EQ(subGroupName, expectedSubGroupNames[sub]);

            const auto subSubGroupInfo = hdf5::lite::groupInfo(path, "/" + groupName + "/" + subGroupName);

            const std::vector<std::string> expectedSubSubGroupNames{"cat", "dog"};
            TEST_ASSERT_EQ(subSubGroupInfo.groups.size(), expectedSubSubGroupNames.size());
            for (size_t subsub = 0; subsub < subSubGroupInfo.groups.size(); subsub++)
            {
                const auto subSubGroupName = subSubGroupInfo.groups[subsub].name;
                TEST_ASSERT_EQ(subSubGroupName, expectedSubSubGroupNames[subsub]);

                const auto datasetPath =  "/" + groupName + "/" + subGroupName + "/" + subSubGroupName;
                read_complex(testName, path, datasetPath);
            }
        }
    }
}

TEST_MAIN(
    TEST_CHECK(test_hdf5Info_IOException);

    TEST_CHECK(test_hdf5FileInfo);
    TEST_CHECK(test_hdf5GroupInfo);
    TEST_CHECK(test_hdf5DatasetInfo);

    TEST_CHECK(test_hdf5Info_nested);
)
