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
#include <random>
#include <std/span>

#include <io/FileInputStream.h>
#include <logging/NullLogger.h>
#include <import/sys.h>

#include <import/six.h>
#include <import/six/sicd.h>
#include <six/sicd/SICDByteProvider.h>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexToAMP8IPHS8I.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/NITFReadComplexXMLControl.h>
#include <six/sicd/KDTree.h>
#include <six/sicd/Utilities.h>

#include "../tests/TestUtilities.h"
#include "TestCase.h"

#if _MSC_VER
#pragma warning(disable: 4459) //  declaration of '...' hides global declaration
#endif

namespace fs = std::filesystem;
using AMP8I_PHS8I_t = six::sicd::ImageData::AMP8I_PHS8I_t;

static std::string testName;

static fs::path argv0()
{
    static const sys::OS os;
    static const fs::path retval = os.getSpecialEnv("0");
    return retval;
}

static fs::path externals_nitro_RelativelPath(const fs::path& filename)
{
    return fs::path("externals") / "nitro" / "modules"/ "c++" / "nitf" / "unittests" / filename;
}

static fs::path getNitfExternalsPath(const fs::path& filename)
{
    const auto root_dir = six::testing::buildRootDir(argv0());
    return root_dir / externals_nitro_RelativelPath(filename);
}

static fs::path nitfPluginRelativelPath()
{
    if (argv0().filename() == "Test.exe") // Google Test in Visual Studio
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
    const auto path = six::testing::buildRootDir(argv0()) / nitfPluginRelativelPath();
    //std::clog << "NITF_PLUGIN_PATH=" << path << "\n";
    sys::OS().setEnv("NITF_PLUGIN_PATH", path.string(), true /*overwrite*/);
}

static std::shared_ptr<six::Container> getContainer(six::sicd::NITFReadComplexXMLControl& reader)
{
    auto container = reader.getContainer();
    TEST_ASSERT_EQ(six::DataType::COMPLEX, container->getDataType());
    TEST_ASSERT_EQ(static_cast<size_t>(1), container->size());
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
    TEST_ASSERT_TRUE(geoData.validate(nullLogger));
    const auto& imageData = *(retval->imageData);
    TEST_ASSERT_TRUE(imageData.validate(geoData, nullLogger));

    const auto& classification = retval->getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    return retval;
}

static void test_nitf_image_info(six::sicd::ComplexData& complexData, const fs::path& inputPathname,
    nitf::PixelValueType expectedPixelValueType)
{
    constexpr auto expectedBlockingMode = nitf::BlockingMode::Pixel;
    constexpr auto expectedImageRepresentation = nitf::ImageRepresentation::NODISPLY;

    const six::NITFImageInfo nitfImageInfo(&complexData);

    auto pixelValueType = nitfImageInfo.getPixelType();
    TEST_ASSERT_EQ(expectedPixelValueType, pixelValueType);

    auto blockingMode = nitfImageInfo.getBlockingMode();
    TEST_ASSERT_EQ(expectedBlockingMode, blockingMode);

    auto imageRepresentation = nitfImageInfo.getImageRepresentation();
    TEST_ASSERT_EQ(expectedImageRepresentation, imageRepresentation);

    nitf::IOHandle io(inputPathname.string());
    nitf::Reader reader;
    const auto record = reader.read(io);
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
            AMP8I_PHS8I_t input(static_cast<uint8_t>(input_amplitude), static_cast<uint8_t>(input_value));
            auto actual = six::sicd::Utilities::from_AMP8I_PHS8I(static_cast<uint8_t>(input_amplitude), static_cast<uint8_t>(input_value), nullptr);

            inputs.push_back(std::move(input));
            expecteds.push_back(std::move(actual));
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
    TEST_ASSERT_EQ(static_cast<size_t>(2), complexData.getNumBytesPerPixel());

    const auto& classification = complexData.getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    const auto& imageData = *(complexData.imageData);
    const auto pAmplitudeTable = imageData.amplitudeTable.get();
    if (pAmplitudeTable != nullptr)
    {
        amplitudeTable = *pAmplitudeTable;
    }

    const auto numBytesPerPixel = complexData.getNumBytesPerPixel();
    TEST_ASSERT_EQ(static_cast<size_t>(2), numBytesPerPixel);

    const auto numChannels = complexData.getNumChannels();
    TEST_ASSERT_EQ(static_cast<size_t>(2), numChannels);

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

static std::vector<std::byte> readFromNITF(const fs::path& inputPathname)
{
    std::unique_ptr<six::sicd::ComplexData> pComplexData;
    auto image = six::sicd::readFromNITF(inputPathname, pComplexData);

    test_assert(*pComplexData, six::PixelType::AMP8I_PHS8I, sizeof(AMP8I_PHS8I_t));

    return image;
}
static std::vector<std::byte> readFromNITF(const fs::path& inputPathname, six::PixelType pixelType)
{
    if (pixelType == six::PixelType::AMP8I_PHS8I)
    {
        return readFromNITF(inputPathname);
    }
    throw std::invalid_argument("Unknown pixelType");
}

TEST_CASE(test_readFromNITF_8_bit_Amp_Phs_Examples)
{
    fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    auto inputPathname = getNitfExternalsPath(filename);
    auto buffer = readFromNITF(inputPathname);

    subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    inputPathname = getNitfExternalsPath(filename);
    buffer = readFromNITF(inputPathname);
}

static six::sicd::ComplexImageResult readSicd_(const fs::path& sicdPathname,
    six::PixelType expectedPixelType, size_t expectedNumBytesPerPixel)
{
    auto result = six::sicd::Utilities::readSicd(sicdPathname);
    test_assert(*(result.pComplexData), expectedPixelType, expectedNumBytesPerPixel);
    return result;
}
static std::vector<std::complex<float>> readSicd(const fs::path& inputPathname)
{
    return readSicd_(inputPathname, six::PixelType::AMP8I_PHS8I, sizeof(AMP8I_PHS8I_t)).widebandData;
}
TEST_CASE(test_read_sicd_8_bit_Amp_Phs_Examples)
{
    fs::path subdir = fs::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    fs::path filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    auto inputPathname = getNitfExternalsPath(filename);
    auto widebandData = readSicd(inputPathname);

    subdir = fs::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    inputPathname = getNitfExternalsPath(filename);
    widebandData = readSicd(inputPathname);
}

template<typename TImage>
static void adjust_image(TImage& image)
{
    // Make it easier to know what we're looking at when examining a binary dump of the SICD
    const auto pImageBytes = six::as_bytes(image);

    pImageBytes[0] = static_cast<std::byte>('[');
    for (size_t i = 1; i < pImageBytes.size() - 1; i++)
    {
        pImageBytes[i] = static_cast<std::byte>('*');
    }
    pImageBytes[pImageBytes.size() - 1] = static_cast<std::byte>(']');
}
static std::vector<std::complex<float>> adjust_image(const six::sicd::ComplexData& complexData,
    std::vector<std::complex<float>>&& image)
{
    if (complexData.getPixelType() != six::PixelType::AMP8I_PHS8I)
    {
        adjust_image(image);
        return image;
    }

    // Convert from AMP8I_PHS8I to that when we convert to AMP8I_PHS8I for writing
    // we'll end up with the "[***...***]" in the file
    void* image_data = image.data();
    std::span<AMP8I_PHS8I_t> from(static_cast<AMP8I_PHS8I_t*>(image_data), getExtent(complexData).area());
    adjust_image(from);

    std::vector<std::complex<float>> retval(from.size());
    complexData.imageData->from_AMP8I_PHS8I(from, retval);
    return retval;
}
static std::vector<std::complex<float>> make_complex_image(const six::sicd::ComplexData& complexData, const types::RowCol<size_t>& dims)
{
    if (complexData.getPixelType() == six::PixelType::AMP8I_PHS8I)
    {
        return adjust_image(complexData, six::sicd::testing::make_complex_image(dims));
    }
    throw std::invalid_argument("Unknown pixelType");
}

template<typename T>
static void test_assert_eq(std::span<const std::byte> bytes, const std::vector<T>& rawData)
{
    const auto rawDataSizeInBytes = rawData.size() * sizeof(rawData[0]);
    TEST_ASSERT_EQ(bytes.size(), rawDataSizeInBytes);

    const auto rawDataBytes = six::as_bytes(rawData);
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

    const auto bytes = readFromNITF(path, pixelType);
    test_assert_eq(expectedBytes, bytes);

    six::sicd::NITFReadComplexXMLControl reader;
    reader.load(path);
    auto container = getContainer(reader);

    const auto pComplexData = getComplexData(*container, 0);
    auto& complexData = *pComplexData;
    test_assert(complexData, pixelType, expectedNumBytesPerPixel);

    const auto extent = getExtent(complexData);
    const types::RowCol<size_t> offset{ 0, 0 };

    if (pixelType == six::PixelType::AMP8I_PHS8I)
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
    const auto result = readSicd_(path, pixelType, expectedNumBytesPerPixel);
    TEST_ASSERT(result.widebandData == image);

    const auto bytes = six::sicd::testing::to_bytes(result);
    read_raw_data(path, pixelType, bytes);
}

static void buffer_list_save(const fs::path& outputName, const std::vector<std::complex<float>>& image,
    std::unique_ptr<six::sicd::ComplexData>&& pComplexData)
{
    six::XMLControlFactory::getInstance().addCreator<six::sicd::ComplexXMLControl>();
    six::NITFWriteControl writer(std::unique_ptr<six::Data>(std::move(pComplexData)));

    static const std::vector<std::string> schemaPaths;
    save(writer, image.data(), outputName.string(), schemaPaths); // API for Python; it uses six::BufferList
}

static void save(const fs::path& outputName, const std::vector<std::complex<float>>& image,
    std::unique_ptr<six::sicd::ComplexData>&& pComplexData)
{
    static const std::vector<fs::path> fs_schemaPaths;
    six::sicd::writeAsNITF(outputName, fs_schemaPaths, *pComplexData, image);
}

template<typename TSave>
static void test_create_sicd_from_mem_(const fs::path& outputName, six::PixelType pixelType, bool makeAmplitudeTable,
    TSave save)
{
    const types::RowCol<size_t> dims(2, 2);

    auto pComplexData = six::sicd::Utilities::createFakeComplexData(pixelType, makeAmplitudeTable, &dims);

    const auto expectedNumBytesPerPixel = pixelType == six::PixelType::RE32F_IM32F ? 8 : (pixelType == six::PixelType::AMP8I_PHS8I ? 2 : -1);
    test_assert(*pComplexData, pixelType, expectedNumBytesPerPixel);
    TEST_ASSERT_EQ(dims.row, pComplexData->getNumRows());
    TEST_ASSERT_EQ(dims.col, pComplexData->getNumCols());

    const auto image = make_complex_image(*pComplexData, dims);
    save(outputName, image, std::move(pComplexData));
    read_nitf(outputName, pixelType, image);
}
static void test_create_sicd_from_mem(const fs::path& outputName, six::PixelType pixelType, bool makeAmplitudeTable = false)
{
    test_create_sicd_from_mem_(outputName, pixelType, makeAmplitudeTable, save);
    test_create_sicd_from_mem_(outputName, pixelType, makeAmplitudeTable, buffer_list_save);
}

TEST_CASE(test_create_sicd_from_mem_8i)
{
    setNitfPluginPath();

    test_create_sicd_from_mem("test_create_sicd_from_mem_8i_amp.sicd", six::PixelType::AMP8I_PHS8I, true /*makeAmplitudeTable*/);
    test_create_sicd_from_mem("test_create_sicd_from_mem_8i_noamp.sicd", six::PixelType::AMP8I_PHS8I, false /*makeAmplitudeTable*/);
}

namespace str
{
inline std::ostream & operator<<(std::ostream & os, const six::sicd::ImageData::AMP8I_PHS8I_t & p)
{
    os << p.first << p.second;
    return os;
}
}
template<typename TNearestNeighbor>
static void test_near_point(const std::complex<float>& p, const six::sicd::ImageData::AMP8I_PHS8I_t& expected,
    TNearestNeighbor nearest_neighbor_f)
{
    auto actual = nearest_neighbor_f(p);
    TEST_ASSERT_EQ(expected, actual);

    actual = nearest_neighbor_f(p + std::complex<float>(0.0f, 0.0f));
    TEST_ASSERT_EQ(expected, actual);
    actual = nearest_neighbor_f(p + std::complex<float>(0.1f, 0.0f));
    TEST_ASSERT_EQ(expected, actual);
    actual = nearest_neighbor_f(p + std::complex<float>(0.0f, 0.1f));
    TEST_ASSERT_EQ(expected, actual);
    actual = nearest_neighbor_f(p + std::complex<float>(0.1f, 0.1f));
    TEST_ASSERT_EQ(expected, actual);

    actual = nearest_neighbor_f(p - std::complex<float>(0.0f, 0.0f));
    TEST_ASSERT_EQ(expected, actual);
    actual = nearest_neighbor_f(p - std::complex<float>(0.1f, 0.0f));
    TEST_ASSERT_EQ(expected, actual);
    actual = nearest_neighbor_f(p - std::complex<float>(0.0f, 0.1f));
    TEST_ASSERT_EQ(expected, actual);
    actual = nearest_neighbor_f(p - std::complex<float>(0.1f, 0.1f));
    TEST_ASSERT_EQ(expected, actual);
}

TEST_CASE(test_KDTree)
{
    using KDNode = six::sicd::ImageData::KDNode;

    const KDNode node0{ {0.0, 0.0}, {static_cast<uint8_t>(0), static_cast<uint8_t>(0)} };
    const KDNode node1{ {1.0, 1.0},  {static_cast<uint8_t>(1), static_cast<uint8_t>(1)} };
    const KDNode node2{ {1.0, -1.0},  {static_cast<uint8_t>(2), static_cast<uint8_t>(2)} };
    const KDNode node3{ {-1.0, 1.0},  {static_cast<uint8_t>(3), static_cast<uint8_t>(3)} };
    const KDNode node4{ {-1.0, -1.0},  {static_cast<uint8_t>(4), static_cast<uint8_t>(4)} };

    std::vector<KDNode> nodes{ node0, node1, node2, node3, node4 };
    const six::sicd::KDTree tree(std::move(nodes));
    const auto nearest_neighbor_f = [&tree](const std::complex<float>& v)
    {
        auto result = tree.nearest_neighbor(six::sicd::ImageData::KDNode{ v });
        return result.amp_and_value;
    };

    test_near_point(node0.result, node0.amp_and_value, nearest_neighbor_f);
    test_near_point(node1.result, node1.amp_and_value, nearest_neighbor_f);
    test_near_point(node2.result, node2.amp_and_value, nearest_neighbor_f);
    test_near_point(node3.result, node3.amp_and_value, nearest_neighbor_f);
    test_near_point(node4.result, node4.amp_and_value, nearest_neighbor_f);

    test_near_point({ 100.0f, 100.0f }, node1.amp_and_value, nearest_neighbor_f); // closest to {1.0, 1.0}
    test_near_point({ 100.0f, -100.0f }, node2.amp_and_value, nearest_neighbor_f); // closest to {1.0, -1.0}
    test_near_point({ -100.0f, 100.0f }, node3.amp_and_value, nearest_neighbor_f); // closest to {-1.0, 1.0}
    test_near_point({ -100.0f, -100.0f }, node4.amp_and_value, nearest_neighbor_f); // closest to {-1.0, -1.0}
}

TEST_CASE(test_verify_phase_uint8_ordering)
{
    // Verify that the uint8 phase values are ordered and evenly spaced from [0, 2PI).
    // If this fails, then a core assumption of the ComplexToAmpPhase8I structure is wrong.

    auto to_phase = [](int v) {
        double p = std::arg(six::sicd::Utilities::from_AMP8I_PHS8I(1, v, nullptr));
        if(p < 0) p += 2.0 * M_PI;
        return p;
    };
    auto p0 = to_phase(0);
    auto p1 = to_phase(1);
    TEST_ASSERT_EQ(p0, 0.0);

    auto delta = p1 - p0;
    auto last = p0;
    for(int i = 1; i < 256; i++) {
        auto check = to_phase(i);
        TEST_ASSERT_ALMOST_EQ_EPS(delta, check - last, 1e-6);
        last = check;
    }
    TEST_ASSERT_ALMOST_EQ_EPS(last + delta, M_PI * 2, 1e-6);
}

TEST_CASE(test_ComplexToAMP8IPHS8I)
{
    // Set up a converter that has a fake amplitude table.
    six::AmplitudeTable amp;
    for(size_t i = 0; i < 256; i++) {
        amp.index(i) = static_cast<double>(i) + 10.0;
    }
    six::sicd::ComplexToAMP8IPHS8I item(&amp);

    // Generate the full 256x256 matrix of possible AMP8I_PHS8I values.
    struct Pairs {
        std::complex<double> floating;
        six::sicd::ImageData::AMP8I_PHS8I_t integral;
    };
    std::vector<Pairs> candidates;
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            Pairs p;
            p.integral = {i, j};
            p.floating = six::sicd::Utilities::from_AMP8I_PHS8I(i, j, &amp);
            candidates.push_back(p);
        }
    }

    // Loop through each item in our matrix and verify that it's mapped correctly.
    // These are simple cases that don't necessarily exercise the nearest neighbor property.
    for(auto& i : candidates) {
        auto truth = i.integral;
        auto test = item.nearest_neighbor(std::complex<float>(i.floating.real(), i.floating.imag()));
        TEST_ASSERT_EQ(truth.first, test.first);
        TEST_ASSERT_EQ(truth.second, test.second);
    }

    // Run an edge case that's very close to a phase of 2PI.
    // The phase should have wrapped back around to 0.
    std::complex<float> problem {
        1, -1e-4
    };
    TEST_ASSERT_EQ(item.nearest_neighbor(problem).second, 0);

    // Verify the nearest neighbor property via random search through the possible space.
    // For each sampled point we check that we found the true nearest neighbor.
    static const size_t kTests = 10000;
    static const double kExpansion = 10.0;
    double min_amplitude = amp.index(0) - kExpansion;
    double max_amplitude = amp.index(amp.numEntries - 1) + kExpansion;
    std::uniform_real_distribution<double> dist(min_amplitude, max_amplitude);
    std::default_random_engine eng(654987);  // ... fixed seed means deterministic tests...
    size_t bad_first = 0;
    size_t bad_second = 0;
    double worst_error = 0;
    for(size_t k = 0; k < kTests; k++) {
        double x = dist(eng);
        double y = dist(eng);

        // Calculate the nearest neighbor quickly.
        const std::complex<double> input_dbl(x, y);
        const auto test_integral = item.nearest_neighbor(input_dbl);

        // Calculate the nearest neighbor via exhaustive calculation.
        double min_distance = std::abs(candidates[0].floating - input_dbl);
        auto best = candidates[0];
        for(auto& i : candidates) {
            double e = std::abs(i.floating - input_dbl);
            if(e < min_distance) {
                min_distance = e;
                best = i;
            }
        }
        TEST_ASSERT_EQ(test_integral.first, best.integral.first);
        TEST_ASSERT_EQ(test_integral.second, best.integral.second);
    }
}

TEST_MAIN((void)argc; (void)argv;
    TEST_CHECK(test_8bit_ampphs);
    TEST_CHECK(read_8bit_ampphs_with_table);
    TEST_CHECK(read_8bit_ampphs_no_table);
    TEST_CHECK(test_readFromNITF_8_bit_Amp_Phs_Examples);
    TEST_CHECK(test_read_sicd_8_bit_Amp_Phs_Examples);
    TEST_CHECK(test_create_sicd_from_mem_8i);
    TEST_CHECK(test_KDTree);
    TEST_CHECK(test_verify_phase_uint8_ordering);
    TEST_CHECK(test_ComplexToAMP8IPHS8I);
    )

