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
#include <numeric>
#include <future>

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
#include <six/sicd/Utilities.h>

#include "../tests/TestUtilities.h"
#include "TestCase.h"

#if _MSC_VER
#pragma warning(disable: 4459) //  declaration of '...' hides global declaration
#endif

using AMP8I_PHS8I_t = six::sicd::AMP8I_PHS8I_t;

static std::shared_ptr<six::Container> getContainer(six::sicd::NITFReadComplexXMLControl& reader)
{
    static const std::string testName("test_AMP8I_PHS8I");
    auto container = reader.getContainer();
    TEST_ASSERT_EQ(six::DataType::COMPLEX, container->getDataType());
    TEST_ASSERT_EQ(static_cast<size_t>(1), container->size());
    return container;
}

static std::unique_ptr<six::sicd::ComplexData> getComplexData(const six::Container& container, size_t jj)
{
    static const std::string testName("test_AMP8I_PHS8I");
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

static void test_nitf_image_info(const std::string& testName,
    six::sicd::ComplexData& complexData, const std::filesystem::path& inputPathname,
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

static void test_assert_eq(const std::string& testName,
    const std::vector<std::complex<float>>& actuals, const std::vector<AMP8I_PHS8I_t>& amp8i_phs8i)
{
    TEST_ASSERT_EQ(actuals.size(), amp8i_phs8i.size());
    for (size_t i = 0; i < actuals.size(); i++)
    {
        const auto& v = amp8i_phs8i[i];
        const auto S = six::sicd::Utilities::from_AMP8I_PHS8I(v.first, v.second, nullptr);
        const std::complex<float> result(gsl::narrow_cast<float>(S.real()), gsl::narrow_cast<float>(S.imag()));
        const auto& expected = actuals[i];
        TEST_ASSERT_EQ(expected, result);
    }
}

static void from_AMP8I_PHS8I(const six::sicd::ImageData& imageData,
    const std::vector<AMP8I_PHS8I_t>& inputs_, std::vector<std::complex<float>>& results_, ptrdiff_t cutoff = -1)
{
    const std::span<const AMP8I_PHS8I_t> inputs(inputs_.data(), inputs_.size());
    const std::span<std::complex<float>> results(results_.data(), results_.size());
    imageData.from_AMP8I_PHS8I(inputs, results, cutoff);
}

static void to_AMP8I_PHS8I(const six::sicd::ImageData& imageData,
    const std::vector<std::complex<float>>& inputs_, std::vector<AMP8I_PHS8I_t>& results_, ptrdiff_t cutoff = -1)
{
    const std::span<const std::complex<float>> inputs(inputs_.data(), inputs_.size());
    const std::span<AMP8I_PHS8I_t> results(results_.data(), results_.size());
    imageData.to_AMP8I_PHS8I(inputs, results, cutoff);
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
            const auto S = six::sicd::Utilities::from_AMP8I_PHS8I(static_cast<uint8_t>(input_amplitude), static_cast<uint8_t>(input_value), nullptr);

            inputs.push_back(std::move(input));
            expecteds.emplace_back(gsl::narrow_cast<float>(S.real()), gsl::narrow_cast<float>(S.imag()));
        }
    }

    std::vector<std::complex<float>> actuals(inputs.size());
    from_AMP8I_PHS8I(imageData, inputs, actuals);
    TEST_ASSERT(actuals == expecteds);


    // we should now be able to convert the cx_floats back to amp/value
    std::vector<AMP8I_PHS8I_t> amp8i_phs8i(actuals.size());
    to_AMP8I_PHS8I(imageData, actuals, amp8i_phs8i);
    test_assert_eq(testName, actuals, amp8i_phs8i);

    // ... and again, async
    const auto cutoff = actuals.size() / 10; // be sure std::async is called
    to_AMP8I_PHS8I(imageData, actuals, amp8i_phs8i, cutoff);
    test_assert_eq(testName, actuals, amp8i_phs8i);
}

static std::vector <std::complex<float>> read_8bit_ampphs(const std::string& testName,
    const std::filesystem::path& inputPathname,
    std::optional<six::AmplitudeTable>& amplitudeTable, std::unique_ptr<six::sicd::ComplexData>& pResultComplexData,
    std::complex<long double> expected_sum)
{
    auto result_ = six::sicd::Utilities::readSicd(inputPathname);
    auto retval = std::move(result_.widebandData);
    pResultComplexData = std::move(result_.pComplexData);

    std::complex<long double> actual_sum;
    for (const auto& cx : retval)
    {
        actual_sum += cx;
    }
    TEST_ASSERT_ALMOST_EQ(actual_sum.real(), expected_sum.real());
    TEST_ASSERT_ALMOST_EQ(actual_sum.imag(), expected_sum.imag());

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

    test_nitf_image_info(testName, complexData, inputPathname, nitf::PixelValueType::Integer);

    return retval;
}

static std::pair<uint64_t, uint64_t> to_AMP8I_PHS8I(const six::sicd::ImageData& imageData, const std::vector<std::complex<float>>& widebandData)
{
    // image is far too big to call to_AMP8I_PHS8I() with DEBUG code
    const auto size = sys::debug ? widebandData.size() / 200 : widebandData.size();
    const std::span<const std::complex<float>> widebandData_(widebandData.data(), size);
    std::vector<AMP8I_PHS8I_t> results(widebandData_.size());
    imageData.to_AMP8I_PHS8I(widebandData_, std::span< AMP8I_PHS8I_t>(results.data(), results.size()), 0);

    std::pair<uint64_t, uint64_t> retval(0, 0);
    for (const auto& r : results)
    {
        retval.first += r.first;
        retval.second += r.second;
    }
    return retval;
}
TEST_CASE(read_8bit_ampphs_with_table)
{
    const auto subdir = std::filesystem::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    const auto filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    const auto inputPathname = six::testing::getNitroPath(filename);

    std::optional<six::AmplitudeTable> amplitudeTable;
    std::unique_ptr<six::sicd::ComplexData> pComplexData;
    const std::complex<long double> expected_sum(-193324027.52878466, -3688020.3755293526);
    const auto widebandData = read_8bit_ampphs(testName, inputPathname, amplitudeTable, pComplexData, expected_sum);

    TEST_ASSERT_TRUE(amplitudeTable.has_value());
    const auto& AmpTable = amplitudeTable.value();
    for (size_t i = 0; i < AmpTable.size(); i++)
    {
        // be sure we don't have garbage data
        TEST_ASSERT_TRUE(std::isfinite(AmpTable.index(i)));
    }

    six::sicd::ImageData imageData;
    imageData.amplitudeTable.reset(std::make_unique< six::AmplitudeTable>(AmpTable));
    const auto actual = to_AMP8I_PHS8I(imageData, widebandData);
    const auto expected(sys::debug ? 
        std::pair<uint64_t, uint64_t>(12647523, 16973148) : std::pair<uint64_t, uint64_t>(3044868397, 3394353166));
    //TEST_ASSERT_EQ(actual.first, expected.first); // TODO
    TEST_ASSERT_EQ(actual.second, expected.second);
}
TEST_CASE(read_8bit_ampphs_no_table)
{
    const auto subdir = std::filesystem::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    const auto filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    const auto inputPathname = six::testing::getNitroPath(filename);

    std::optional<six::AmplitudeTable> amplitudeTable;
    std::unique_ptr<six::sicd::ComplexData> pComplexData;
    const std::complex<long double> expected_sum(-12398989.376837999, 397846.88834372163);
    const auto widebandData = read_8bit_ampphs(testName, inputPathname, amplitudeTable, pComplexData, expected_sum);
    TEST_ASSERT_FALSE(amplitudeTable.has_value());

    six::sicd::ImageData imageData;
    const auto actual = to_AMP8I_PHS8I(imageData, widebandData);
    const auto expected(sys::debug ?
        std::pair<uint64_t, uint64_t>(12647654, 16973148) : std::pair<uint64_t, uint64_t>(3044873160, 3394353122));
    TEST_ASSERT_EQ(actual.first, expected.first);
    TEST_ASSERT_EQ(actual.second, expected.second);
}

static void test_assert(const six::sicd::ComplexData& complexData,
    six::PixelType expectedPixelType, size_t expectedNumBytesPerPixel)
{
    static const std::string testName("test_AMP8I_PHS8I");

    TEST_ASSERT_EQ(expectedPixelType, complexData.getPixelType());

    const auto& classification = complexData.getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    const auto numBytesPerPixel = complexData.getNumBytesPerPixel();
    TEST_ASSERT_EQ(expectedNumBytesPerPixel, numBytesPerPixel);
}

static std::vector<std::byte> readFromNITF(const std::filesystem::path& inputPathname)
{
    std::unique_ptr<six::sicd::ComplexData> pComplexData;
    auto image = six::sicd::readFromNITF(inputPathname, pComplexData);

    test_assert(*pComplexData, six::PixelType::AMP8I_PHS8I, sizeof(AMP8I_PHS8I_t));

    return image;
}
static std::vector<std::byte> readFromNITF(const std::filesystem::path& inputPathname, six::PixelType pixelType)
{
    if (pixelType == six::PixelType::AMP8I_PHS8I)
    {
        return readFromNITF(inputPathname);
    }
    throw std::invalid_argument("Unknown pixelType");
}

TEST_CASE(test_readFromNITF_8_bit_Amp_Phs_Examples)
{
    auto subdir = std::filesystem::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    auto filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    auto inputPathname = six::testing::getNitroPath(filename);
    auto buffer = readFromNITF(inputPathname);

    subdir = std::filesystem::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    inputPathname = six::testing::getNitroPath(filename);
    buffer = readFromNITF(inputPathname);
}

static six::sicd::ComplexImageResult readSicd_(const std::filesystem::path& sicdPathname,
    six::PixelType expectedPixelType, size_t expectedNumBytesPerPixel)
{
    auto result = six::sicd::Utilities::readSicd(sicdPathname);
    test_assert(*(result.pComplexData), expectedPixelType, expectedNumBytesPerPixel);
    return result;
}
static std::vector<std::complex<float>> readSicd(const std::filesystem::path& inputPathname)
{
    return readSicd_(inputPathname, six::PixelType::AMP8I_PHS8I, sizeof(AMP8I_PHS8I_t)).widebandData;
}
TEST_CASE(test_read_sicd_8_bit_Amp_Phs_Examples)
{
    auto subdir = std::filesystem::path("8_bit_Amp_Phs_Examples") / "No_amplitude_table";
    auto filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_no_amplitude_table_SICD.nitf";
    auto inputPathname = six::testing::getNitroPath(filename);
    auto widebandData = readSicd(inputPathname);

    subdir = std::filesystem::path("8_bit_Amp_Phs_Examples") / "With_amplitude_table";
    filename = subdir / "sicd_example_1_PFA_AMP8I_PHS8I_VV_with_amplitude_table_SICD.nitf";
    inputPathname = six::testing::getNitroPath(filename);
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
    std::span<AMP8I_PHS8I_t> from_(static_cast<AMP8I_PHS8I_t*>(image_data), getExtent(complexData).area());
    adjust_image(from_);

    std::span<const AMP8I_PHS8I_t> from(from_.data(), from_.size());
    std::vector<std::complex<float>> retval(from.size());
    complexData.imageData->from_AMP8I_PHS8I(from, std::span<std::complex<float>>(retval.data(), retval.size()));
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
    static const std::string testName("test_AMP8I_PHS8I");
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
template<typename T>
static void test_assert_eq(const std::vector<std::byte>& bytes, const std::vector<T>& rawData)
{
    test_assert_eq(std::span<const std::byte>(bytes.data(), bytes.size()), rawData);
}

static void read_raw_data(const std::filesystem::path& path, six::PixelType pixelType, std::span<const std::byte> expectedBytes)
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

static void read_nitf(const std::string& testName,
    const std::filesystem::path& path, six::PixelType pixelType, const std::vector<std::complex<float>>& image)
{
    const auto expectedNumBytesPerPixel = pixelType == six::PixelType::RE32F_IM32F ? 8 : (pixelType == six::PixelType::AMP8I_PHS8I ? 2 : -1);
    const auto result = readSicd_(path, pixelType, expectedNumBytesPerPixel);
    TEST_ASSERT(result.widebandData == image);

    const auto bytes = six::sicd::testing::to_bytes(result);
    read_raw_data(path, pixelType, std::span<const std::byte>(bytes.data(), bytes.size()));
}

static void buffer_list_save(const std::filesystem::path& outputName, const std::vector<std::complex<float>>& image,
    std::unique_ptr<six::sicd::ComplexData>&& pComplexData)
{
    six::XMLControlFactory::getInstance().addCreator<six::sicd::ComplexXMLControl>();
    six::NITFWriteControl writer(std::unique_ptr<six::Data>(std::move(pComplexData)));

    static const std::vector<std::string> schemaPaths;
    six::save(writer, image.data(), outputName.string(), schemaPaths); // API for Python; it uses six::BufferList
}

static void save(const std::filesystem::path& outputName, const std::vector<std::complex<float>>& image,
    std::unique_ptr<six::sicd::ComplexData>&& pComplexData)
{
    static const std::vector<std::filesystem::path> fs_schemaPaths;
    six::sicd::writeAsNITF(outputName, fs_schemaPaths, *pComplexData, std::span<const std::complex<float>>(image.data(), image.size()));
}

static void test_assert_image_(const std::string& testName,
    const std::vector<std::complex<float>>& image, const six::sicd::ComplexData& complexData)
{
    static const std::vector<std::complex<float>> expected_cxfloat{
        std::complex<float>(46.7833481f, 78.0533066f),
        std::complex<float>(21.5923157f, 36.0246010f),
        std::complex<float>(21.5923157f, 36.0246010f),
        std::complex<float>(-27.4332600f, 31.8027706f) };
    TEST_ASSERT_EQ(image.size(), expected_cxfloat.size());
    for (size_t i = 0; i < image.size(); i++)
    {
        TEST_ASSERT_ALMOST_EQ(image[i].real(), expected_cxfloat[i].real());
        TEST_ASSERT_ALMOST_EQ(image[i].imag(), expected_cxfloat[i].imag());
    }

    const std::span<const std::complex<float>> input(image.data(), image.size());
    std::vector<AMP8I_PHS8I_t> result(input.size());
    std::span< AMP8I_PHS8I_t> result_(result.data(), result.size());
    complexData.imageData->to_AMP8I_PHS8I(input, result_);

    static const std::vector<AMP8I_PHS8I_t> expected_amp8i_phs8i{
        AMP8I_PHS8I_t(91, 42), AMP8I_PHS8I_t(42, 42), AMP8I_PHS8I_t(42, 42), AMP8I_PHS8I_t(42, 93) }; // "[******]"
    for (size_t i = 0; i < result.size(); i++)
    {
        TEST_ASSERT_EQ(result[i].first, expected_amp8i_phs8i[i].first);
        TEST_ASSERT_EQ(result[i].second, expected_amp8i_phs8i[i].second);
    }
}

template<typename TSave>
static void test_create_sicd_from_mem_(const std::string& testName,
    const std::filesystem::path& outputName, six::PixelType pixelType, bool makeAmplitudeTable,
    TSave save)
{
    const types::RowCol<size_t> dims(2, 2);

    auto pComplexData = six::sicd::Utilities::createFakeComplexData("1.2.1", pixelType, makeAmplitudeTable, &dims);

    const auto expectedNumBytesPerPixel = pixelType == six::PixelType::RE32F_IM32F ? 8 : (pixelType == six::PixelType::AMP8I_PHS8I ? 2 : -1);
    test_assert(*pComplexData, pixelType, expectedNumBytesPerPixel);
    TEST_ASSERT_EQ(dims.row, pComplexData->getNumRows());
    TEST_ASSERT_EQ(dims.col, pComplexData->getNumCols());

    const auto image = make_complex_image(*pComplexData, dims);
    TEST_ASSERT_EQ(image.size(), dims.area());
    test_assert_image_(testName, image, *pComplexData);

    save(outputName, image, std::move(pComplexData));
    read_nitf(testName, outputName, pixelType, image);
}
static void test_create_sicd_from_mem(const std::string& testName,
    const std::filesystem::path& outputName, six::PixelType pixelType, bool makeAmplitudeTable = false)
{
    test_create_sicd_from_mem_(testName, outputName, pixelType, makeAmplitudeTable, save);
    test_create_sicd_from_mem_(testName, outputName, pixelType, makeAmplitudeTable, buffer_list_save);
}

TEST_CASE(test_create_sicd_from_mem_8i)
{
    test_create_sicd_from_mem(testName, "test_create_sicd_from_mem_8i_amp.sicd", six::PixelType::AMP8I_PHS8I, true /*makeAmplitudeTable*/);
    test_create_sicd_from_mem(testName, "test_create_sicd_from_mem_8i_noamp.sicd", six::PixelType::AMP8I_PHS8I, false /*makeAmplitudeTable*/);
}

static void test_adjusted_values(const std::string& testName, const std::vector<std::complex<float>>& values,
    const std::vector<AMP8I_PHS8I_t>& expected, std::complex<float> delta)
{
    auto adjusted_values = values;
    for (auto& v : adjusted_values)
    {
        v += delta;
    }
    std::vector<AMP8I_PHS8I_t> actual(expected.size());
    std::span<AMP8I_PHS8I_t> actual_(actual.data(), actual.size());
    std::span<const std::complex<float>> values_(adjusted_values.data(), adjusted_values.size());
    six::sicd::ImageData::to_AMP8I_PHS8I(nullptr /*pAmplitudeTable*/, values_, actual_);
    for (size_t i = 0; i < expected.size(); i++)
    {
        TEST_ASSERT_EQ(expected[i].first, actual[i].first);
        TEST_ASSERT_EQ(expected[i].second, actual[i].second);
    }
}

TEST_CASE(test_nearest_neighbor)
{
    const std::vector<std::complex<float>> values{
        {0.0, 0.0}, {1.0, 1.0}, {10.0, -10.0}, {-100.0, 100.0}, {-1000.0, -1000.0} };

    const std::vector<AMP8I_PHS8I_t> expected{
        {static_cast<uint8_t>(0), static_cast<uint8_t>(0)},
        {static_cast<uint8_t>(1), static_cast<uint8_t>(32)},
        {static_cast<uint8_t>(14), static_cast<uint8_t>(224)},
        {static_cast<uint8_t>(141), static_cast<uint8_t>(96)},
        {static_cast<uint8_t>(255), static_cast<uint8_t>(160)} };

    std::vector<AMP8I_PHS8I_t> actual(expected.size());
    std::span<AMP8I_PHS8I_t> actual_(actual.data(), actual.size());
    std::span<const std::complex<float>> values_(values.data(), values.size());
    six::sicd::ImageData::to_AMP8I_PHS8I(nullptr /*pAmplitudeTable*/, values_, actual_);

    for (size_t i = 0; i < expected.size(); i++)
    {
        TEST_ASSERT_EQ(expected[i].first, actual[i].first);
        TEST_ASSERT_EQ(expected[i].second, actual[i].second);
    }

    auto other_expected = expected;

    constexpr auto delta = 0.0122f;
    test_adjusted_values(testName, values, other_expected,  std::complex<float>(delta, 0.0f));

    other_expected[0].second = 32;
    test_adjusted_values(testName, values, other_expected, std::complex<float>(delta, delta));

    other_expected[0].second += 32;
    test_adjusted_values(testName, values, other_expected, std::complex<float>(0.0f, delta));

    other_expected[0].second += 32;
    test_adjusted_values(testName, values, other_expected, std::complex<float>(-delta, delta));

    other_expected[0].second += 32;
    test_adjusted_values(testName, values, other_expected, std::complex<float>(-delta, 0.0f));

    other_expected[0].second += 32;
    test_adjusted_values(testName, values, other_expected, std::complex<float>(-delta, -delta));

    other_expected[0].second += 32;
    test_adjusted_values(testName, values, other_expected, std::complex<float>(0.0f, -delta));

    other_expected[0].second += 32;
    test_adjusted_values(testName, values, other_expected, std::complex<float>(delta, -delta));

    other_expected[0].second += 32;
    TEST_ASSERT_EQ(other_expected[0].second, expected[0].second);
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

struct Pairs final
{
    std::complex<float> floating;
    AMP8I_PHS8I_t integral;
};
static void do_test_ComplexToAMP8IPHS8I_(const std::string& testName,
    const six::sicd::details::ComplexToAMP8IPHS8I& item,
    const std::complex<float>& input_dbl, const std::vector<Pairs>& candidates)
{
    // Calculate the nearest neighbor quickly.
    const auto test_integral = item.nearest_neighbor(input_dbl);

    // Calculate the nearest neighbor via exhaustive calculation.
    double min_distance = std::abs(candidates[0].floating - input_dbl);
    auto best = candidates[0];
    for (const auto& i : candidates)
    {
        const auto e = std::abs(i.floating - input_dbl);
        if (e < min_distance)
        {
            min_distance = e;
            best = i;
        }
    }
    TEST_ASSERT_EQ(test_integral.first, best.integral.first);
    TEST_ASSERT_EQ(test_integral.second, best.integral.second);
}
using it_t = std::vector<std::complex<float>>::const_iterator;
static void test_ComplexToAMP8IPHS8I_(const std::string& testName,
    const six::sicd::details::ComplexToAMP8IPHS8I& item,
    it_t beg, it_t end, const std::vector<Pairs>& candidates)
{
    for (auto it = beg; it != end; ++it)
    {
        do_test_ComplexToAMP8IPHS8I_(testName, item, *it, candidates);
    }
}
static void test_ComplexToAMP8IPHS8I(const std::string& testName,
    const six::sicd::details::ComplexToAMP8IPHS8I& item,
    it_t beg, it_t end, const std::vector<Pairs>& candidates)
{
    // https://en.cppreference.com/w/cpp/thread/async
    const auto len = end - beg;
    if (len < 500)
    {
        test_ComplexToAMP8IPHS8I_(testName, item, beg, end, candidates);
        return;
    }

    const auto mid = beg + len / 2;
    static const auto f = [&](it_t mid, it_t end) { test_ComplexToAMP8IPHS8I(testName, item, mid, end, candidates); };
    auto handle = std::async(std::launch::async, f, mid, end);
    f(beg, mid);
    handle.get();
}

TEST_CASE(test_ComplexToAMP8IPHS8I)
{
    // Set up a converter that has a fake amplitude table.
    six::AmplitudeTable amplitudeTable; // "amp" is a (somewhat) reserved with MSVC
    for(size_t i = 0; i < 256; i++)
    {
        amplitudeTable.index(i) = static_cast<double>(i) + 10.0;
    }    
    std::unique_ptr<six::sicd::details::ComplexToAMP8IPHS8I> pTree; // not-cached, non-NULL amplitudeTable
    const auto& item = *(six::sicd::details::ComplexToAMP8IPHS8I::make(&amplitudeTable, pTree));

    // Generate the full 256x256 matrix of possible AMP8I_PHS8I values.
    std::vector<Pairs> candidates;
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            Pairs p;
            p.integral = {i, j};
            p.floating = six::sicd::Utilities::from_AMP8I_PHS8I(i, j, &amplitudeTable);
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
        1.0f, -1e-4f
    };
    TEST_ASSERT_EQ(item.nearest_neighbor(problem).second, 0);

    // Verify the nearest neighbor property via random search through the possible space.
    // For each sampled point we check that we found the true nearest neighbor.
    constexpr size_t kTests = 10000;
    constexpr double kExpansion = 10.0;
    double min_amplitude = amplitudeTable.index(0) - kExpansion;
    double max_amplitude = amplitudeTable.index(amplitudeTable.numEntries - 1) + kExpansion;
    std::uniform_real_distribution<double> dist(min_amplitude, max_amplitude);
    std::default_random_engine eng(654987);  // ... fixed seed means deterministic tests...
    //size_t bad_first = 0;
    //size_t bad_second = 0;
    //double worst_error = 0;
    std::vector<std::complex<float>> inputs;
    for(size_t k = 0; k < kTests; k++)
    {
        double x = dist(eng);
        double y = dist(eng);
        inputs.emplace_back(x, y);
    }
    test_ComplexToAMP8IPHS8I(testName, item, inputs.begin(), inputs.end(), candidates);
}

TEST_MAIN(
    TEST_CHECK(test_8bit_ampphs);
    TEST_CHECK(read_8bit_ampphs_with_table);
    TEST_CHECK(read_8bit_ampphs_no_table);
    TEST_CHECK(test_readFromNITF_8_bit_Amp_Phs_Examples);
    TEST_CHECK(test_read_sicd_8_bit_Amp_Phs_Examples);
    TEST_CHECK(test_create_sicd_from_mem_8i);
    TEST_CHECK(test_nearest_neighbor);
    TEST_CHECK(test_verify_phase_uint8_ordering);
    TEST_CHECK(test_ComplexToAMP8IPHS8I);
    )

