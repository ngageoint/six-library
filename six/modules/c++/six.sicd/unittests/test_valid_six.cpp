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

#include <std/filesystem>

#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <import/sys.h>

#include <import/six.h>
#include <import/six/sicd.h>
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

static fs::path nitfRelativelPath(const fs::path& filename)
{
    return fs::path("six") / "modules" / "c++" / "six" / "tests" / "nitf" / filename;
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

static fs::path getNitfPath(const fs::path& filename)
{
    const auto root_dir = buildRootDir();
    return root_dir / nitfRelativelPath(filename);
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
    //std::clog << "NITF_PLUGIN_PATH=" << path << "\n";
    sys::OS().setEnv("NITF_PLUGIN_PATH", path.string(), true /*overwrite*/);
}

class NITFReader final
{
    // create an XML registry
    // The reason to do this is to avoid adding XMLControlCreators to the
    // XMLControlFactory singleton - this way has more fine-grained control
    six::XMLControlRegistry xmlRegistry;

    // this validates the DES of the input against the best available schema
    six::NITFReadControl reader;

public:
    NITFReader()
    {
        setNitfPluginPath();

        xmlRegistry.addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        // this validates the DES of the input against the best available schema
        reader.setXMLControlRegistry(&xmlRegistry);
    }

    mem::SharedPtr<const six::Container> load(const fs::path& fromFile)
    {
        std::vector<std::string> schemaPaths;
        reader.load(fromFile.string(), schemaPaths);
        return reader.getContainer();
    }
};

static std::unique_ptr<six::sicd::ComplexData> getComplexData(const six::Container& container, size_t jj)
{
    std::unique_ptr<six::Data> data_;
    data_.reset(container.getData(jj)->clone());

    TEST_ASSERT_EQ(six::DataType::COMPLEX, data_->getDataType());
    std::unique_ptr<six::sicd::ComplexData> retval(dynamic_cast<six::sicd::ComplexData*>(data_.release()));

    logging::NullLogger nullLogger;
    //TEST_ASSERT_TRUE(retval->validate(nullLogger));
    const auto& geoData = *(retval->geoData);
    //TEST_ASSERT_TRUE(geoData.validate(nullLogger));
    const auto& imageData = *(retval->imageData);
    TEST_ASSERT_TRUE(imageData.validate(geoData, nullLogger));

    return retval;
}

TEST_CASE(valid_six_50x50)
{
    const auto inputPathname = getNitfPath("sicd_50x50.nitf");

    NITFReader reader;
    auto container = reader.load(inputPathname);
    TEST_ASSERT_EQ(container->getNumData(), 1);
    for (size_t jj = 0; jj < container->getNumData(); ++jj)
    {
        const auto data = getComplexData(*container, jj);
        TEST_ASSERT_EQ(six::PixelType::RE32F_IM32F, data->getPixelType());
        TEST_ASSERT_EQ(8, data->getNumBytesPerPixel());

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

        //const auto& imageData = *(data->imageData);
    }
}

TEST_CASE(read_8bit_ampphs_with_table)
{
    const fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    const fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    const auto inputPathname = getNitfPath(filename);

    NITFReader reader;
    auto container = reader.load(inputPathname);
    TEST_ASSERT_EQ(container->getNumData(), 1);
    for (size_t jj = 0; jj < container->getNumData(); ++jj)
    {
        const auto data = getComplexData(*container, jj);
        TEST_ASSERT_EQ(six::PixelType::AMP8I_PHS8I, data->getPixelType());
        TEST_ASSERT_EQ(2, data->getNumBytesPerPixel());

        const auto& classification = data->getClassification();
        TEST_ASSERT_TRUE(classification.isUnclassified());

        const auto& imageData = *(data->imageData);
        const auto pAmplitudeTable = imageData.amplitudeTable.get();
        TEST_ASSERT(pAmplitudeTable != nullptr);
    }
}

TEST_CASE(read_8bit_ampphs_no_table)
{
    const fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    const fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    const auto inputPathname = getNitfPath(filename);

    NITFReader reader;
    auto container = reader.load(inputPathname);
    TEST_ASSERT_EQ(container->getNumData(), 1);
    for (size_t jj = 0; jj < container->getNumData(); ++jj)
    {
        const auto data = getComplexData(*container, jj);
        TEST_ASSERT_EQ(six::PixelType::AMP8I_PHS8I, data->getPixelType());
        TEST_ASSERT_EQ(2, data->getNumBytesPerPixel());

        const auto& classification = data->getClassification();
        TEST_ASSERT_TRUE(classification.isUnclassified());

        const auto& imageData = *(data->imageData);
        const auto pAmplitudeTable = imageData.amplitudeTable.get();
        TEST_ASSERT_EQ(nullptr, pAmplitudeTable);
    }
}

TEST_MAIN((void)argc;
    argv0 = fs::absolute(argv[0]);
    TEST_CHECK(valid_six_50x50);
    TEST_CHECK(read_8bit_ampphs_with_table);    
    TEST_CHECK(read_8bit_ampphs_no_table);
    )

