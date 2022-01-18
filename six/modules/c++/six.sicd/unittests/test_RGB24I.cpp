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

static six::sicd::ComplexImageResult readSicd_(const fs::path& sicdPathname,
    six::PixelType expectedPixelType, size_t expectedNumBytesPerPixel)
{
    auto result = six::sicd::Utilities::readSicd(sicdPathname);
    test_assert(*(result.pComplexData), expectedPixelType, expectedNumBytesPerPixel);
    return result;
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
static std::vector<std::complex<float>> adjust_image(std::vector<std::complex<float>>&& image)
{
        adjust_image(image);
        return image;
}
static std::vector<std::complex<float>> make_complex_image(const six::sicd::ComplexData& complexData, const types::RowCol<size_t>& dims)
{
    if (complexData.getPixelType() == six::PixelType::RGB24I)
    {
        return adjust_image(six::sicd::testing::make_complex_image(dims));
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
static void test_create_sicd_from_mem_(const fs::path& outputName, TSave save)
{
    const types::RowCol<size_t> dims(2, 2);

    auto pComplexData = six::sicd::Utilities::createFakeComplexData(six::PixelType::RGB24I, false /*makeAmplitudeTable*/, &dims);

    constexpr size_t expectedNumBytesPerPixel = 3;
    test_assert(*pComplexData, six::PixelType::RGB24I, expectedNumBytesPerPixel);
    TEST_ASSERT_EQ(dims.row, pComplexData->getNumRows());
    TEST_ASSERT_EQ(dims.col, pComplexData->getNumCols());

    const auto image = make_complex_image(*pComplexData, dims);
    save(outputName, image, std::move(pComplexData));
    read_nitf(outputName, six::PixelType::RGB24I, image);
}
static void test_create_sicd_from_mem(const fs::path& outputName)
{
    test_create_sicd_from_mem_(outputName, save);
    test_create_sicd_from_mem_(outputName, buffer_list_save);
}

TEST_CASE(test_create_sicd_from_mem_24i)
{
    setNitfPluginPath();

    test_create_sicd_from_mem("test_create_sicd_from_mem_rgb24i.sicd");
}

TEST_MAIN((void)argc; (void)argv;
    TEST_CHECK(test_create_sicd_from_mem_24i);
    )

