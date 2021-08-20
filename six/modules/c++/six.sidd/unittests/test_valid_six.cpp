/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <stdlib.h>

#include <string>
#include <iostream>
#include <string>

#include <std/filesystem>

#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <import/sys.h>

#include <import/six.h>
#include <import/six/sidd.h>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sidd/Utilities.h>

#include "../tests/TestUtilities.h"
#include "TestCase.h"

namespace fs = std::filesystem;

static fs::path argv0;
static const fs::path file = __FILE__;
static std::string testName;

static bool is_linux()
{
    const auto cpp = file.filename().stem(); // i.e., "test_valid_six"
    const auto exe = argv0.filename(); // e.g., "test_valid_six.exe"
    return cpp == exe; // no ".exe", must be Linux
}

static bool is_vs_gtest()
{
    return argv0.empty(); // no argv[0] in VS w/GTest
}

static fs::path buildRootDir()
{
    const auto cpp = file.filename().stem(); // i.e., "test_valid_six"
    const auto exe = argv0.filename(); // e.g., "test_valid_six.exe"
    if (is_linux())
    {
        const auto root_dir = argv0.parent_path().parent_path().parent_path().parent_path().parent_path().parent_path().parent_path();
        return root_dir;
    }

    // On Windows ... in Visual Studio or stand-alone?
    if (is_vs_gtest())
    {
        const auto cwd = fs::current_path();
        const auto root_dir = cwd.parent_path().parent_path();
        return root_dir;
    }
    else
    {
        // stand-alone
        const auto root_dir = argv0.parent_path().parent_path().parent_path().parent_path().parent_path().parent_path().parent_path();
        return root_dir;
    }
}

static fs::path nitfPluginRelativelPath()
{
    if (is_vs_gtest())
    {
        static const sys::OS os;
        static const std::string configuration = os.getSpecialEnv("Configuration");
        static const std::string platform = os.getSpecialEnv("Platform");
        return fs::path("externals") / "nitro" / platform / configuration / "share" / "nitf" / "plugins";
    }

    //return fs::path("install") / "share" / "six.sicd" / "conf" / "schema";
    return fs::path("install") / "share" / "CSM" / "plugins";
}
static void setNitfPluginPath()
{
    const auto path = buildRootDir() / nitfPluginRelativelPath();
    //std::clog << "NITF_PLUGIN_PATH=" << path << "\n";
    sys::OS().setEnv("NITF_PLUGIN_PATH", path.string(), true /*overwrite*/);
}

static void test_create_sidd_from_mem(const fs::path& /*outputName*/, six::PixelType /*pixelType*/)
{
    //const types::RowCol<size_t> dims(2, 2);

    //constexpr auto dataType = six::DataType::DERIVED;
    //six::XMLControlFactory::getInstance().addCreator(dataType,
    //    new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

    //std::vector<std::complex<float>> image;
    //image.reserve(dims.area());
    //for (size_t r = 0; r < dims.row; r++)
    //{
    //    for (size_t c = 0; c < dims.col; c++)
    //    {
    //        image.push_back(std::complex<float>(r, c * -1.0));
    //    }
    //}

    //std::unique_ptr<six::Data> data(six::sidd::Utilities::createFakeDerivedData(dims));
    //data->setPixelType(pixelType);

    //mem::SharedPtr<six::Container> container(new six::Container(dataType));
    //container->addData(std::move(data));

    //const auto pData = container->getData(0);
    //TEST_ASSERT_EQ(dims.row, pData->getNumRows());
    //TEST_ASSERT_EQ(dims.col, pData->getNumCols());

    //const six::Options writerOptions;
    //six::NITFWriteControl writer(writerOptions, container);

    //const std::vector<std::string> schemaPaths;
    //const void* pImageData = image.data();
    //six::buffer_list buffers{ static_cast<const std::byte*>(pImageData) };
    //writer.save(buffers, outputName.string(), schemaPaths);
}

TEST_CASE(test_create_sidds_from_mem)
{
    setNitfPluginPath();

    test_create_sidd_from_mem("test_create_sicd_from_mem_8i.sicd", six::PixelType::AMP8I_PHS8I);
}

TEST_MAIN((void)argc;
    argv0 = fs::absolute(argv[0]);
    TEST_CHECK(test_create_sidds_from_mem);
    )

