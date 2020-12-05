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

#include <string>

#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <import/sys.h>

#include <import/six.h>
#include <import/six/sicd.h>
#include "TestCase.h"

#include <sys/Filesystem.h>
namespace fs = sys::Filesystem;

static fs::path argv0;
static const fs::path file = __FILE__;

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

static fs::path nitfRelativelPath()
{
    return fs::path("six") / "modules" / "c++" / "six" / "tests" / "nitf" / "sicd_50x50.nitf";
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

static fs::path getNitfPath()
{
    const auto root_dir = buildRootDir();
    return root_dir / nitfRelativelPath();
}

static fs::path nitfPluginRelativelPath()
{
    if (is_vs_gtest())
    {
        const std::string configuration =
#if defined(NDEBUG) // i.e., release
            "Release";
#else
            "Debug";
#endif
        const std::string platform = "x64";
        return fs::path("externals") / "nitro" / platform / configuration / "share" / "nitf" / "plugins";
    }

    //return fs::path("install") / "share" / "six.sicd" / "conf" / "schema";
    return fs::path("install") / "share" / "CSM" / "plugins";
}

static void setNitfPluginPath()
{
    const auto path = buildRootDir() / nitfPluginRelativelPath();
    //std::clog << "NITF_PLUGIN_PATH=" << path.string() << "\n";
    sys::OS().setEnv("NITF_PLUGIN_PATH", path.string(), true /*overwrite*/);
}

TEST_CASE(valid_six_50x50)
{
    setNitfPluginPath();
    const std::string inputPathname = getNitfPath().string(); // sicd_50x50.nitf

    // create an XML registry
    // The reason to do this is to avoid adding XMLControlCreators to the
    // XMLControlFactory singleton - this way has more fine-grained control
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
        new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

    // this validates the DES of the input against the best available schema
    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);

    std::vector<std::string> schemaPaths;
    reader.load(inputPathname, schemaPaths);
    std::shared_ptr<six::Container> container = reader.getContainer();
    TEST_ASSERT_EQ(container->getNumData(), 1);
    for (size_t jj = 0; jj < container->getNumData(); ++jj)
    {
        std::unique_ptr<six::Data> data;
        data.reset(container->getData(jj)->clone());

        TEST_ASSERT(data->getDataType() == six::DataType::COMPLEX);

        const auto& classification = data->getClassification();
        TEST_ASSERT_TRUE(classification.isUnclassified());

        // UTF-8 characters in 50x50.nitf
        #ifdef _WIN32
        const std::string classificationText("NON CLASSIFI\xc9 / UNCLASSIFIED"); // ISO8859-1 "NON CLASSIFIÉ / UNCLASSIFIED"
        #else
        const std::string classificationText("NON CLASSIFI\xc3\x89 / UNCLASSIFIED"); // UTF-8 "NON CLASSIFIÉ / UNCLASSIFIED"
        #endif
        const auto actual = classification.getLevel();
        TEST_ASSERT_EQ(actual, classificationText);
    }
}

TEST_MAIN(
    argv0 = fs::absolute(argv[0]);
    TEST_CHECK(valid_six_50x50);
)

