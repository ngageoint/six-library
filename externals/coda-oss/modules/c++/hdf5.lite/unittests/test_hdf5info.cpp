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

TEST_CASE(test_hdf5Info)
{
    static const auto path = find_unittest_file("example.h5");

    // https://www.mathworks.com/help/matlab/ref/h5info.html
    const auto info = hdf5::lite::fileInfo(path);
}

TEST_MAIN(
    TEST_CHECK(test_hdf5Info);
)
