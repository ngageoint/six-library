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
#include <std/optional>
#include <cmath>
#include <std/span>

#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <import/sys.h>

#include <import/six.h>
#include <import/six/sicd.h>
#include <six/sicd/SICDByteProvider.h>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/NITFReadComplexXMLControl.h>
#include <six/sicd/Utilities.h>

#include "../tests/TestUtilities.h"
#include "TestCase.h"

namespace fs = std::filesystem;
using AMP8I_PHS8I_t = six::sicd::ImageData::AMP8I_PHS8I_t;

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
static fs::path externals_nitro_RelativelPath(const fs::path& filename)
{
    return fs::path("externals") / "nitro" / "modules"/ "c++" / "nitf" / "unittests" / filename;
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

static fs::path getNitfExternalsPath(const fs::path& filename)
{
    const auto root_dir = buildRootDir();
    return root_dir / externals_nitro_RelativelPath(filename);
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

static std::shared_ptr<six::Container> getContainer(six::sicd::NITFReadComplexXMLControl& reader)
{
    auto container = reader.getContainer();
    TEST_ASSERT_EQ(six::DataType::COMPLEX, container->getDataType());
    TEST_ASSERT_EQ(1, container->getNumData());
    return container;
}

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

    const auto& classification = retval->getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    return retval;
}

static void test_nitf_image_info(six::sicd::ComplexData& complexData, const fs::path& inputPathname,
    nitf::PixelValueType expectedPixelValueType)
{
    const auto expectedBlockingMode = nitf::BlockingMode::Pixel;
    const auto expectedImageRepresentation = nitf::ImageRepresentation::NODISPLY;

    const six::NITFImageInfo nitfImageInfo(&complexData);

    auto pixelValueType = nitfImageInfo.getPixelType();
    TEST_ASSERT_EQ(expectedPixelValueType, pixelValueType);

    auto blockingMode = nitfImageInfo.getBlockingMode();
    TEST_ASSERT_EQ(expectedBlockingMode, blockingMode);

    auto imageRepresentation = nitfImageInfo.getImageRepresentation();
    TEST_ASSERT_EQ(expectedImageRepresentation, imageRepresentation);

    nitf::IOHandle io(inputPathname.string());
    nitf::Reader reader;
    nitf::Record record = reader.read(io);
    for (const auto& recordImage : record.getImages())
    {
        const nitf::ImageSegment imageSegment(recordImage);
        const auto subheader = imageSegment.getSubheader();

        pixelValueType = subheader.pixelValueType();
        TEST_ASSERT_EQ(expectedPixelValueType, pixelValueType);

        blockingMode = subheader.imageBlockingMode();
        TEST_ASSERT_EQ(expectedBlockingMode, blockingMode);

        imageRepresentation = subheader.imageRepresentation();
        TEST_ASSERT_EQ(expectedImageRepresentation, imageRepresentation);
    }
}

TEST_CASE(valid_six_50x50)
{
    const auto inputPathname = getNitfPath("sicd_50x50.nitf");
    std::unique_ptr<six::sicd::ComplexData> pComplexData;
    const auto image = six::sicd::readFromNITF(inputPathname, pComplexData);
    const six::Data* pData = pComplexData.get();

    TEST_ASSERT_EQ(six::PixelType::RE32F_IM32F, pData->getPixelType());
    TEST_ASSERT_EQ(8, pData->getNumBytesPerPixel());

    // UTF-8 characters in 50x50.nitf
    #ifdef _WIN32
    const std::string classificationText("NON CLASSIFI\xc9 / UNCLASSIFIED"); // ISO8859-1 "NON CLASSIFIÉ / UNCLASSIFIED"
    #else
    const std::string classificationText("NON CLASSIFI\xc3\x89 / UNCLASSIFIED"); // UTF-8 "NON CLASSIFIÉ / UNCLASSIFIED"
    #endif
    const auto& classification = pData->getClassification();
    const auto actual = classification.getLevel();
    TEST_ASSERT_EQ(actual, classificationText);

    //const auto& imageData = *(data->imageData);

    test_nitf_image_info(*pComplexData, inputPathname, nitf::PixelValueType::Floating);
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

static void test_assert_eq(const std::vector<std::complex<float>>& actuals, const std::vector<AMP8I_PHS8I_t>& amp8i_phs8i)
{
    TEST_ASSERT_EQ(actuals.size(), amp8i_phs8i.size());
    for (size_t i = 0; i < actuals.size(); i++)
    {
        const auto& v = amp8i_phs8i[i];
        const auto result = six::sicd::Utilities::from_AMP8I_PHS8I(v.first, v.second, nullptr);
        const auto& expected = actuals[i];
        TEST_ASSERT_EQ(expected, result);
    }
}

TEST_CASE(test_8bit_ampphs)
{
    six::sicd::ImageData imageData;
    imageData.pixelType = six::PixelType::AMP8I_PHS8I;

    std::vector<AMP8I_PHS8I_t> inputs;
    std::vector<std::complex<float>> expecteds;
    for (uint16_t input_amplitude = 0; input_amplitude <= UINT8_MAX; input_amplitude++)
    {
        for (uint16_t input_value = 0; input_value <= UINT8_MAX; input_value++)
        {
            auto expected = from_AMP8I_PHS8I(input_amplitude, input_value);

            AMP8I_PHS8I_t input(input_amplitude, input_value);
            const auto actual = imageData.from_AMP8I_PHS8I(input);
            TEST_ASSERT_EQ(expected, actual);

            const auto actual_utilities = six::sicd::Utilities::from_AMP8I_PHS8I(input_amplitude, input_value, nullptr);
            TEST_ASSERT_EQ(expected, actual_utilities);
            TEST_ASSERT_EQ(actual_utilities, actual);

            inputs.push_back(std::move(input));
            expecteds.push_back(std::move(expected));
        }
    }

    std::vector<std::complex<float>> actuals(inputs.size());
    imageData.from_AMP8I_PHS8I(inputs, actuals);
    TEST_ASSERT(actuals == expecteds);


    // we should now be able to convert the cx_floats back to amp/value
    std::vector<AMP8I_PHS8I_t> amp8i_phs8i(actuals.size());
    imageData.to_AMP8I_PHS8I(actuals, amp8i_phs8i);
    test_assert_eq(actuals, amp8i_phs8i);

    // ... and again, async
    const auto cutoff = actuals.size() / 10; // be sure std::async is called
    imageData.to_AMP8I_PHS8I(actuals, amp8i_phs8i, cutoff);
    test_assert_eq(actuals, amp8i_phs8i);
}

static std::vector <std::complex<float>> read_8bit_ampphs(const fs::path& inputPathname,
    std::optional<six::AmplitudeTable>& amplitudeTable, std::unique_ptr<six::sicd::ComplexData>& pResultComplexData)
{
    auto result_ = six::sicd::Utilities::readSicd(inputPathname);
    auto retval = std::move(result_.widebandData);
    pResultComplexData = std::move(result_.pComplexData);

    auto& complexData = *pResultComplexData;
    TEST_ASSERT_EQ(six::PixelType::AMP8I_PHS8I, complexData.getPixelType());
    TEST_ASSERT_EQ(2, complexData.getNumBytesPerPixel());

    const auto& classification = complexData.getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    const auto& imageData = *(complexData.imageData);
    const auto pAmplitudeTable = imageData.amplitudeTable.get();
    if (pAmplitudeTable != nullptr)
    {
        amplitudeTable = *pAmplitudeTable;
    }

    const auto numBytesPerPixel = complexData.getNumBytesPerPixel();
    TEST_ASSERT_EQ(2, numBytesPerPixel);

    const auto numChannels = complexData.getNumChannels();
    TEST_ASSERT_EQ(2, numChannels);

    test_nitf_image_info(complexData, inputPathname, nitf::PixelValueType::Integer);

    return retval;
}

static void to_AMP8I_PHS8I(const six::sicd::ImageData& imageData, const std::vector<std::complex<float>>& widebandData)
{
    // image is far too big to call to_AMP8I_PHS8I() with DEBUG code
    const auto size = sys::debug ? widebandData.size() / 200 : widebandData.size();
    std::span<const std::complex<float>> widebandData_(widebandData.data(), size);
    std::vector<AMP8I_PHS8I_t> results(widebandData_.size());
    imageData.to_AMP8I_PHS8I(widebandData_, results, 0);
}
TEST_CASE(read_8bit_ampphs_with_table)
{
    const fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    const fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    const auto inputPathname = getNitfExternalsPath(filename);

    std::optional<six::AmplitudeTable> amplitudeTable;
    std::unique_ptr<six::sicd::ComplexData> pComplexData;
    const auto widebandData = read_8bit_ampphs(inputPathname, amplitudeTable, pComplexData);

    TEST_ASSERT_TRUE(amplitudeTable.has_value());
    const auto& AmpTable = amplitudeTable.value();
    // be sure we don't have garbage data
    for (size_t i = 0; i < AmpTable.size(); i++)
    {
        const auto v = AmpTable.index(i);
        TEST_ASSERT_TRUE(std::isfinite(v));
    }

    six::sicd::ImageData imageData;
    imageData.amplitudeTable.reset(std::make_unique< six::AmplitudeTable>(AmpTable));
    to_AMP8I_PHS8I(imageData, widebandData);
}
TEST_CASE(read_8bit_ampphs_no_table)
{
    const fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    const fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    const auto inputPathname = getNitfExternalsPath(filename);

    std::optional<six::AmplitudeTable> amplitudeTable;
    std::unique_ptr<six::sicd::ComplexData> pComplexData;
    const auto widebandData = read_8bit_ampphs(inputPathname, amplitudeTable, pComplexData);
    TEST_ASSERT_FALSE(amplitudeTable.has_value());

    six::sicd::ImageData imageData;
    to_AMP8I_PHS8I(imageData, widebandData);
}

static void test_assert(const six::sicd::ComplexData& complexData,
    six::PixelType expectedPixelType, size_t expectedNumBytesPerPixel)
{
    TEST_ASSERT_EQ(expectedPixelType, complexData.getPixelType());

    const auto& classification = complexData.getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    const auto numBytesPerPixel = complexData.getNumBytesPerPixel();
    TEST_ASSERT_EQ(expectedNumBytesPerPixel, numBytesPerPixel);
}

static std::vector<std::byte> readFromNITF_(const fs::path& inputPathname,
    six::PixelType expectedPixelType, size_t expectedNumBytesPerPixel)
{
    std::unique_ptr<six::sicd::ComplexData> pComplexData;
    auto image = six::sicd::readFromNITF(inputPathname, pComplexData);

    test_assert(*pComplexData, expectedPixelType, expectedNumBytesPerPixel);

    return image;
}
template<typename T> std::vector<std::byte> readFromNITF(const fs::path& inputPathname);
template<> std::vector<std::byte> readFromNITF<std::complex<float>>(const fs::path& inputPathname)
{
    return readFromNITF_(inputPathname, six::PixelType::RE32F_IM32F, sizeof(std::complex<float>));
}
template<> std::vector<std::byte> readFromNITF<AMP8I_PHS8I_t>(const fs::path& inputPathname)
{
    return readFromNITF_(inputPathname, six::PixelType::AMP8I_PHS8I, sizeof(AMP8I_PHS8I_t));
}
TEST_CASE(sicd_readFromNITF)
{
    auto inputPathname = getNitfPath("sicd_50x50.nitf");
    auto buffer = readFromNITF<std::complex<float>>(inputPathname);

    fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    inputPathname = getNitfExternalsPath(filename);
    buffer = readFromNITF<AMP8I_PHS8I_t>(inputPathname);

    subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    inputPathname = getNitfExternalsPath(filename);
    buffer = readFromNITF<AMP8I_PHS8I_t>(inputPathname);
}

static std::vector<std::complex<float>> readSicd_(const fs::path& sicdPathname,
    six::PixelType expectedPixelType, size_t expectedNumBytesPerPixel)
{
    auto result = six::sicd::Utilities::readSicd(sicdPathname);
    test_assert(*(result.pComplexData), expectedPixelType, expectedNumBytesPerPixel);
    return result.widebandData;
}
template<typename T> std::vector<std::complex<float>> readSicd(const fs::path& inputPathname);
template<> std::vector<std::complex<float>> readSicd<std::complex<float>>(const fs::path& inputPathname)
{
    return readSicd_(inputPathname, six::PixelType::RE32F_IM32F, sizeof(std::complex<float>));
}
template<> std::vector<std::complex<float>> readSicd<AMP8I_PHS8I_t>(const fs::path& inputPathname)
{
    return readSicd_(inputPathname, six::PixelType::AMP8I_PHS8I, sizeof(AMP8I_PHS8I_t));
}
TEST_CASE(test_readSicd)
{
    auto inputPathname = getNitfPath("sicd_50x50.nitf");
    auto widebandData = readSicd<std::complex<float>>(inputPathname);

    fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    inputPathname = getNitfExternalsPath(filename);
    widebandData = readSicd<AMP8I_PHS8I_t>(inputPathname);

    subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    inputPathname = getNitfExternalsPath(filename);
    widebandData = readSicd<AMP8I_PHS8I_t>(inputPathname);
}

static std::vector<std::complex<float>> make_complex_image_(const types::RowCol<size_t>& dims)
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
static std::vector<std::complex<float>> make_complex_image_(std::vector<std::complex<float>>&& image)
{
    void* image_data = image.data();
    // Make it easier to know what we're looking at when examining a binary dump of the SICD
    std::span<std::byte> pImage(static_cast<std::byte*>(image_data), image.size() * sizeof(image[0]));
    TEST_ASSERT_EQ(32, pImage.size());

    pImage[0] = static_cast<std::byte>('[');
    for (size_t i = 1; i < pImage.size() - 1; i++)
    {
        pImage[i] = static_cast<std::byte>('*');
    }
    pImage[pImage.size() - 1] = static_cast<std::byte>(']');

    return image;
}
static std::vector<std::complex<float>> make_complex_image(const types::RowCol<size_t>& dims, six::PixelType pixelType)
{
    if (pixelType == six::PixelType::RE32F_IM32F)
    {
        return make_complex_image_(dims);
        //return make_complex_image_(make_complex_image_(dims));
    }
    if (pixelType == six::PixelType::AMP8I_PHS8I)
    {
        return make_complex_image_(make_complex_image_(dims));
    }
    throw std::invalid_argument("Unknown pixelType");
}

template<typename T>
static void test_assert_eq(std::span<const std::byte> bytes, const std::vector<T>& rawData)
{
    const auto rawDataSizeInBytes = rawData.size() * sizeof(rawData[0]);
    TEST_ASSERT_EQ(bytes.size(), rawDataSizeInBytes);

    const void* pRawData_ = rawData.data();
    auto pRawData = static_cast<const std::byte*>(pRawData_);
    std::span<const std::byte> rawDataBytes(pRawData, rawDataSizeInBytes);
    TEST_ASSERT_EQ(bytes.size(), rawDataBytes.size());
    for (size_t i = 0; i < bytes.size(); i++)
    {
        const auto bytes_i = static_cast<uint8_t>(bytes[i]);
        const auto rawDataBytes_i = static_cast<uint8_t>(rawDataBytes[i]);
        TEST_ASSERT_EQ(bytes_i, rawDataBytes_i);
    }
}

static void read_raw_data(const fs::path& path, six::PixelType pixelType, std::span<const std::byte> expectedBytes)
{
    const auto expectedNumBytesPerPixel = pixelType == six::PixelType::RE32F_IM32F ? 8 : (pixelType == six::PixelType::AMP8I_PHS8I ? 2 : -1);

    const auto bytes = readFromNITF_(path, pixelType, expectedNumBytesPerPixel);
    test_assert_eq(expectedBytes, bytes);

    six::sicd::NITFReadComplexXMLControl reader;
    reader.load(path);
    auto container = getContainer(reader);

    const auto pComplexData = getComplexData(*container, 0);
    auto& complexData = *pComplexData;
    test_assert(complexData, pixelType, expectedNumBytesPerPixel);

    const auto extent = getExtent(complexData);
    const types::RowCol<size_t> offset{ 0, 0 };

    if (pixelType == six::PixelType::RE32F_IM32F)
    {
        std::vector<std::complex<float>> rawData;
        six::sicd::Utilities::getRawData(reader.NITFReadControl(), complexData, offset, extent, rawData);
        test_assert_eq(bytes, rawData);
        test_assert_eq(expectedBytes, rawData);
    }
    else if (pixelType == six::PixelType::AMP8I_PHS8I)
    {
        std::vector<AMP8I_PHS8I_t> rawData;
        six::sicd::Utilities::getRawData(reader.NITFReadControl(), complexData, offset, extent, rawData);
        test_assert_eq(bytes, rawData);
        test_assert_eq(expectedBytes, rawData);
    }
}
static void read_nitf(const fs::path& path, six::PixelType pixelType, const std::vector<std::complex<float>>& image)
{
    const auto expectedNumBytesPerPixel = pixelType == six::PixelType::RE32F_IM32F ? 8 : (pixelType == six::PixelType::AMP8I_PHS8I ? 2 : -1);
    const auto widebandData = readSicd_(path, pixelType, expectedNumBytesPerPixel);
    TEST_ASSERT(widebandData == image);

    const void* pImage = image.data();
    std::span<const std::byte> bytes(static_cast<const std::byte*>(pImage), image.size() * sizeof(image[0]));
    read_raw_data(path, pixelType, bytes);
}
static void test_create_sicd_from_mem(const fs::path& outputName, six::PixelType pixelType, bool makeAmplitudeTable=false)
{
    const types::RowCol<size_t> dims(2, 2);

    auto pComplexData = six::sicd::Utilities::createFakeComplexData(pixelType, makeAmplitudeTable, &dims);

    const auto expectedNumBytesPerPixel = pixelType == six::PixelType::RE32F_IM32F ? 8 : (pixelType == six::PixelType::AMP8I_PHS8I ? 2 : -1);
    test_assert(*pComplexData, pixelType, expectedNumBytesPerPixel);
    TEST_ASSERT_EQ(dims.row, pComplexData->getNumRows());
    TEST_ASSERT_EQ(dims.col, pComplexData->getNumCols());

    const auto image = make_complex_image(dims, pixelType);

    static const std::vector<fs::path> schemaPaths;
    //six::sicd::writeAsNITF(outputName, schemaPaths, *pComplexData, image.data());
    six::XMLControlFactory::getInstance().addCreator<six::sicd::ComplexXMLControl>();
    auto container = std::make_shared<six::Container>(std::move(pComplexData));
    const six::Options writerOptions;
    six::NITFWriteControl writer(writerOptions, container);
    writer.save(image, outputName, schemaPaths);

    read_nitf(outputName, pixelType, image);
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
    TEST_CHECK(sicd_readFromNITF);
    TEST_CHECK(test_readSicd);
    TEST_CHECK(test_create_sicds_from_mem);
    )

