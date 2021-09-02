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
#include <utility>

#include <std/filesystem>

#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <import/sys.h>

#include <import/six.h>
#include <import/six/sicd.h>
#include <six/sicd/SICDByteProvider.h>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>

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
    TEST_ASSERT_EQ(1, container->getNumData());

    const auto data = getComplexData(*container, 0);
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

static std::complex<float> from_AMP8I_PHS8I(uint8_t input_amplitude, uint8_t input_value)
{
    // A = input_amplitude(i.e. 0 to 255)
    const double A = input_amplitude;

    // The phase values should be read in (values 0 to 255) and converted to float by doing:
    // P = (1 / 256) * input_value
    const double P = (1.0 / 256.0) * input_value;

    // To convert the amplitude and phase values to complex float (i.e. real and imaginary):
    // S = A * cos(2 * pi * P) + j * A * sin(2 * pi * P)
    const auto angle = 2 * M_PI * P;
    const auto real = A * cos(angle);
    const auto imaginary = A * sin(angle);
    return std::complex<float>(gsl::narrow_cast<float>(real), gsl::narrow_cast<float>(imaginary));
}

TEST_CASE(test_8bit_ampphs)
{
    six::sicd::ImageData imageData;
    imageData.pixelType = six::PixelType::AMP8I_PHS8I;

    std::vector<six::sicd::ImageData::AMP8I_PHS8I_t> inputs;
    std::vector<std::complex<float>> expecteds;
    for (uint16_t input_amplitude = 0; input_amplitude <= UINT8_MAX; input_amplitude++)
    {
        for (uint16_t input_value = 0; input_value <= UINT8_MAX; input_value++)
        {
            auto expected = from_AMP8I_PHS8I(input_amplitude, input_value);

            six::sicd::ImageData::AMP8I_PHS8I_t input(input_amplitude, input_value);
            const auto actual = imageData.from_AMP8I_PHS8I(input);
            TEST_ASSERT_EQ(expected, actual);

            const auto actual_utilities = six::sicd::Utilities::from_AMP8I_PHS8I(input_amplitude, input_value, nullptr);
            TEST_ASSERT_EQ(expected, actual_utilities);
            TEST_ASSERT_EQ(actual_utilities, actual);

            inputs.push_back(std::move(input));
            expecteds.push_back(std::move(expected));
        }
    }

    const auto actuals = imageData.from_AMP8I_PHS8I(inputs);
    TEST_ASSERT(actuals == expecteds);


    // we should now be able to convert the cx_floats back to amp/value
    const auto amp8i_phs8i = imageData.to_AMP8I_PHS8I(actuals);
    TEST_ASSERT_EQ(actuals.size(), amp8i_phs8i.size());
    for (size_t i = 0; i < actuals.size(); i++)
    {
        const auto& v = amp8i_phs8i[i];
        const auto result = six::sicd::Utilities::from_AMP8I_PHS8I(v.first, v.second, nullptr);
        const auto& expected = actuals[i];
        TEST_ASSERT_EQ(expected, result);
    }
}

TEST_CASE(read_8bit_ampphs_with_table)
{
    const fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    const fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    const auto inputPathname = getNitfPath(filename);

    NITFReader reader;
    auto container = reader.load(inputPathname);
    TEST_ASSERT_EQ(1, container->getNumData());

    const auto data = getComplexData(*container, 0);
    TEST_ASSERT_EQ(six::PixelType::AMP8I_PHS8I, data->getPixelType());
    TEST_ASSERT_EQ(2, data->getNumBytesPerPixel());

    const auto& classification = data->getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    const auto& imageData = *(data->imageData);
    const auto pAmplitudeTable = imageData.amplitudeTable.get();
    TEST_ASSERT(pAmplitudeTable != nullptr);

    const auto& AmpTable = *pAmplitudeTable;
    for (size_t i = 0; i <= UINT8_MAX; i++)
    {
        const auto v = AmpTable.index(i);
        TEST_ASSERT_EQ(v, (v / 1.0) + 0.0); // be sure it's not something goofy like NaN, Inf, etc.
    }
}

TEST_CASE(read_8bit_ampphs_no_table)
{
    const fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    const fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    const auto inputPathname = getNitfPath(filename);

    NITFReader reader;
    auto container = reader.load(inputPathname);
    TEST_ASSERT_EQ(1, container->getNumData());

    const auto data = getComplexData(*container, 0);
    TEST_ASSERT_EQ(six::PixelType::AMP8I_PHS8I, data->getPixelType());
    TEST_ASSERT_EQ(2, data->getNumBytesPerPixel());

    const auto& classification = data->getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    const auto& imageData = *(data->imageData);
    const auto pAmplitudeTable = imageData.amplitudeTable.get();
    TEST_ASSERT_EQ(nullptr, pAmplitudeTable);
}

static std::vector<std::byte> sicd_read_data_(const fs::path& inputPathname,
    six::PixelType expectedPixelType, size_t expectedNumBytesPerPixel)
{
    // create an XML registry
    // The reason to do this is to avoid adding XMLControlCreators to the
    // XMLControlFactory singleton - this way has more fine-grained control
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX, new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);

    static const std::vector<std::string> schemaPaths;
    reader.load(inputPathname.string(), schemaPaths);
    auto container = reader.getContainer();
    TEST_ASSERT_EQ(six::DataType::COMPLEX, container->getDataType());

    // For SICD, there's only one image (container->getNumData() == 1)
    TEST_ASSERT_EQ(1, container->getNumData());
    constexpr size_t imageNumber = 0;
    auto pComplexData = getComplexData(*container, imageNumber);
    auto& complexData = *pComplexData;
    TEST_ASSERT_EQ(expectedPixelType, complexData.getPixelType());

    const auto& classification = complexData.getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    const auto numBytesPerPixel = complexData.getNumBytesPerPixel();
    TEST_ASSERT_EQ(expectedNumBytesPerPixel, numBytesPerPixel);

    const auto extent = getExtent(complexData);
    const auto numPixels = extent.area();
    std::vector<std::byte> buffer_(numPixels * numBytesPerPixel);
    auto buffer = buffer_.data();

    six::Region region;
    setDims(region, extent);
    constexpr size_t offset = 0;
    region.setBuffer(buffer + offset);
    const auto pData = reader.interleaved(region, imageNumber);
    TEST_ASSERT_NOT_EQ(nullptr, pData);
    return buffer_;
}
static void sicd_read_data(const fs::path& inputPathname,
    const std::complex<float>& expectedFirstPixel, const std::complex<float>& expectedLastPixel)
{
    const auto buffer = sicd_read_data_(inputPathname, six::PixelType::RE32F_IM32F, sizeof(expectedFirstPixel));

    const auto firstPixel = std::complex<float>(0);
    TEST_ASSERT_EQ(expectedFirstPixel, firstPixel);

    const auto lastPixel = std::complex<float>(0);
    TEST_ASSERT_EQ(expectedLastPixel, lastPixel);
}
static void sicd_read_data(const fs::path& inputPathname,
    int16_t expectedFirstPixel, int16_t expectedLastPixel)
{
    const auto buffer = sicd_read_data_(inputPathname, six::PixelType::AMP8I_PHS8I, sizeof(expectedFirstPixel));

    const int16_t firstPixel = 0;
    TEST_ASSERT_EQ(expectedFirstPixel, firstPixel);

    const int16_t lastPixel = 0;
    TEST_ASSERT_EQ(expectedLastPixel, lastPixel);
}
TEST_CASE(sicd_read_data)
{
    auto inputPathname = getNitfPath("sicd_50x50.nitf");
    sicd_read_data(inputPathname, std::complex<float>(0), std::complex<float>(0));

    fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    inputPathname = getNitfPath(filename);
    sicd_read_data(inputPathname, 0, 0);

    subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    inputPathname = getNitfPath(filename);
    sicd_read_data(inputPathname, 0, 0);
}

TEST_CASE(test_readSicd)
{
    static const std::vector<std::string> schemaPaths;

    auto inputPathname = getNitfPath("sicd_50x50.nitf");
    six::sicd::ComplexData complexData;
    auto widebandData = six::sicd::Utilities::readSicd(inputPathname, schemaPaths, complexData);

    fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    inputPathname = getNitfPath(filename);
    widebandData = six::sicd::Utilities::readSicd(inputPathname, schemaPaths, complexData);

    subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    inputPathname = getNitfPath(filename);
    widebandData = six::sicd::Utilities::readSicd(inputPathname, schemaPaths, complexData);
}

static std::vector<std::complex<float>> make_complex_image(const types::RowCol<size_t>& dims)
{
    std::vector<std::complex<float>> image;
    image.reserve(dims.area());
    for (size_t r = 0; r < dims.row; r++)
    {
        for (size_t c = 0; c < dims.col; c++)
        {
            image.push_back(std::complex<float>(r, c * -1.0));
        }
    }
    return image;
}
static std::vector<std::pair<uint8_t, uint8_t>> make_AMP8I_PHS8I_image(const types::RowCol<size_t>& dims, const six::AmplitudeTable*)
{
    std::vector<std::pair<uint8_t, uint8_t>> image;
    for (size_t r = 0; r < dims.row; r++)
    {
        for (size_t c = 0; c < dims.col; c++)
        {
            image.push_back(std::pair<uint8_t, uint8_t>(r, c));
        }
    }
    return image;
}

struct image final
{
    std::vector<std::complex<float>> RE32F_IM32F;
    std::vector<std::pair<uint8_t, uint8_t>> AMP8I_PHS8I;

    const std::byte* data() const
    {
        const void* retval = nullptr;
        if (!RE32F_IM32F.empty())
        {
            retval = RE32F_IM32F.data();
            assert(AMP8I_PHS8I.empty());
        }
        else if (!AMP8I_PHS8I.empty())
        {
            retval = AMP8I_PHS8I.data();
            assert(RE32F_IM32F.empty());
        }
        return static_cast<const std::byte*>(retval);
    }
};

static image make_image(const types::RowCol<size_t>& dims, const six::Data& data)
{
    const auto pixelType = data.getPixelType();
    six::AmplitudeTable* pAmpTable = pixelType == six::PixelType::AMP8I_PHS8I ? data.getAmplitudeTable() : nullptr;

    image retval;
    if (pixelType == six::PixelType::RE32F_IM32F)
    {
        retval.RE32F_IM32F = make_complex_image(dims);
    }
    else if (pixelType == six::PixelType::AMP8I_PHS8I)
    {
        retval.AMP8I_PHS8I = make_AMP8I_PHS8I_image(dims, pAmpTable);
    }
    else
    {
        throw std::invalid_argument("Unknown 'pixelType'.");
    }

    return retval;
}

static void test_create_sicd_from_mem(const fs::path& outputName, six::PixelType pixelType, bool makeAmplitudeTable=false)
{
    const types::RowCol<size_t> dims(2, 2);

    constexpr auto dataType = six::DataType::COMPLEX;
    six::XMLControlFactory::getInstance().addCreator(dataType, new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

    std::unique_ptr<six::Data> data = six::sicd::Utilities::createFakeComplexData(pixelType, makeAmplitudeTable, &dims);
    const auto image = make_image(dims, *data);

    mem::SharedPtr<six::Container> container(new six::Container(dataType));
    container->addData(std::move(data));

    const auto pData = container->getData(0);
    TEST_ASSERT_EQ(dims.row, pData->getNumRows());
    TEST_ASSERT_EQ(dims.col, pData->getNumCols());

    const six::Options writerOptions;
    six::NITFWriteControl writer(writerOptions, container);

    const std::vector<std::string> schemaPaths;
    six::buffer_list buffers{ image.data() };
    writer.save(buffers, outputName.string(), schemaPaths);
}

TEST_CASE(test_create_sicds_from_mem)
{
    setNitfPluginPath();

    test_create_sicd_from_mem("test_create_sicd_from_mem_32f.sicd", six::PixelType::RE32F_IM32F);
    //test_create_sicd_from_mem("test_create_sicd_from_mem_8i_amp.sicd", six::PixelType::AMP8I_PHS8I, true /*makeAmplitudeTable*/);
    //test_create_sicd_from_mem("test_create_sicd_from_mem_8i_noamp.sicd", six::PixelType::AMP8I_PHS8I, false /*makeAmplitudeTable*/);
}

TEST_MAIN((void)argc;
    argv0 = fs::absolute(argv[0]);
    TEST_CHECK(valid_six_50x50);
    TEST_CHECK(test_8bit_ampphs);
    TEST_CHECK(read_8bit_ampphs_with_table);
    TEST_CHECK(read_8bit_ampphs_no_table);
    TEST_CHECK(sicd_read_data);
    TEST_CHECK(test_readSicd);
    TEST_CHECK(test_create_sicds_from_mem);
    )

