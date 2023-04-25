/* =========================================================================
 * This file is part of hdf5.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2023, Maxar Technologies, Inc.
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
#include <numeric>
#include <tuple>

#include <TestCase.h>

#include "sys/FileFinder.h"

#include "hdf5/lite/Write.h"
#include "hdf5/lite/HDF5Exception.h"
#include "hdf5/lite/Info.h"
#include "hdf5/lite/Read.h"

static std::filesystem::path find_unittest_file(const std::filesystem::path& name)
{
    static const auto unittests = std::filesystem::path("modules") / "c++" / "hdf5.lite" / "unittests";
    return sys::test::findGITModuleFile("coda-oss", unittests, name);
}

TEST_CASE(test_hdf5Create)
{
    static const auto path_ = find_unittest_file("example.h5");
    static const auto path = path_.parent_path() / "TEST_hdf5Create_TMP.h5";

    // https://www.mathworks.com/help/matlab/ref/h5write.html
    hdf5::lite::createFile<float>(path, "/DS1", {10, 20});

    TEST_ASSERT_TRUE(true); // need to use hidden "testName" parameter
}

TEST_CASE(test_hdf5Write)
{
    static const auto path_ = find_unittest_file("example.h5");
    static const auto path = path_.parent_path() / "TEST_hdf5Write_TMP.h5";

    const types::RowCol<size_t> dims{10, 20};
    std::vector<double> data_(dims.area());
    const hdf5::lite::SpanRC<double> data(data_.data(), dims);
    double d = 0.0;
    for (size_t r = 0; r<dims.row; r++)
    {
        for (size_t c = 0; c < dims.col; c++)
        {
            data(r, c) = d++;
        }    
    }

    // https://www.mathworks.com/help/matlab/ref/h5write.html
    hdf5::lite::createFile(path, "/DS1", data);
    hdf5::lite::writeFile(path, "/DS1", data);

    // Be sure we can read the file just written
    const auto info = hdf5::lite::dataSetInfo(path, "/DS1");
    TEST_ASSERT_EQ(path.string(), info.filename);
    TEST_ASSERT_EQ("DS1", info.name);
    TEST_ASSERT(info.dataType.h5Class == hdf5::lite::Class::Float);

    std::vector<double> result;
    const auto rc = hdf5::lite::readFile(path, "/DS1", result);
    TEST_ASSERT(rc.dims() == dims);
    TEST_ASSERT_EQ(dims.area(), result.size());
    for (size_t i = 0; i < result.size(); i++)
    {
        const auto expected = static_cast<double>(i);
        TEST_ASSERT_EQ(result[i], expected);
    }
}

TEST_MAIN(
    TEST_CHECK(test_hdf5Create);
    TEST_CHECK(test_hdf5Write);
)
