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

static std::filesystem::path find_unittest_file(const std::filesystem::path& name)
{
    static const auto unittests = std::filesystem::path("modules") / "c++" / "hdf5.lite" / "unittests";
    return sys::test::findGITModuleFile("coda-oss", unittests, name);
}

TEST_CASE(test_hdf5Read)
{
    static const auto path = find_unittest_file("example.h5");

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

TEST_CASE(test_hdf5Read_nested)
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

    // https://www.mathworks.com/help/matlab/ref/h5read.html
    std::vector<double> data; // TODO: float
    auto rc = hdf5::lite::readFile(path, "/1/bar/cat/i", data);
    TEST_ASSERT_EQ(rc.area(), 10);
    TEST_ASSERT_EQ(rc.area(), data.size());

    rc = hdf5::lite::readFile(path, "/3/foo/dog/r", data);
    TEST_ASSERT_EQ(rc.area(), 10);
    TEST_ASSERT_EQ(rc.area(), data.size());
}

TEST_MAIN(
    TEST_CHECK(test_hdf5Read);
    TEST_CHECK(test_hdf5Read_IOException);
    TEST_CHECK(test_hdf5Read_nested);
)
