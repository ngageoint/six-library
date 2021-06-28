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
#include <import/six/sicd.h>
#include <six/sicd/SICDByteProvider.h>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>

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

class NITFWriter final
{
    static constexpr auto dataType = six::DataType::COMPLEX;
    mem::SharedPtr<six::Container> container{ new six::Container(dataType) };
    six::XMLControlRegistry xmlRegistry;

    std::unique_ptr<six::NITFWriteControl> pWriter;
    std::unique_ptr<const six::sicd::SICDByteProvider> pSicdByteProvider;

    const std::vector<std::string> mSchemaPaths;

public:
    NITFWriter(const six::sicd::ComplexData& data, const six::Options& options, const std::vector<std::string>& schemaPaths)
        : mSchemaPaths(schemaPaths)
    {
        container->addData(data.clone());
        xmlRegistry.addCreator(dataType, new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());
        pWriter = std::make_unique<six::NITFWriteControl>(options, container, &xmlRegistry);
    }
    NITFWriter(const six::sicd::ComplexData& data, const std::vector<std::string>& schemaPaths, size_t maxProductSize = 0)
        : pSicdByteProvider(std::make_unique<six::sicd::SICDByteProvider>(data, schemaPaths, maxProductSize))
    {
    }

    void save(const six::buffer_list& buffers, const std::string& pathname) const
    {
        pWriter->save(buffers, pathname, mSchemaPaths);
    }
    template<typename DataTypeT>
    void save(const std::vector<std::complex<DataTypeT>>& image, const std::string& pathname) const
    {
        six::buffer_list buffers;
        buffers.push_back(reinterpret_cast<const std::byte*>(image.data()));
        save(buffers, pathname);
    }

    bool write(io::FileOutputStream& outStream, const void* imageData, size_t startRow, size_t numRows) const
    {
        nitf::Off fileOffset;
        nitf::NITFBufferList buffers;
        pSicdByteProvider->getBytes(imageData, startRow, numRows, fileOffset, buffers);
        const auto computedNumBytes = pSicdByteProvider->getNumBytes(startRow, numRows);

        outStream.seek(fileOffset, io::Seekable::START);
        nitf::Off numBytes(0);
        for (const auto& buffer : buffers.mBuffers)
        {
            outStream.write(static_cast<const std::byte*>(buffer.mData), buffer.mNumBytes);
            numBytes += buffer.mNumBytes;
        }

        return numBytes == computedNumBytes;
    }
    bool write(const std::string& path, const void* imageData, size_t startRow, size_t numRows) const
    {
        io::FileOutputStream outStream(path);
        const auto retval = write(outStream, imageData, startRow, numRows);
        outStream.close();
        return retval;
    }
};

template<typename DataTypeT>
static std::vector<std::complex<DataTypeT>> createBigEndianImage(std::vector< std::complex<DataTypeT>>& image)
{
    for (size_t ii = 0; ii < image.size(); ++ii)
    {
        image[ii] = std::complex<DataTypeT>(static_cast<DataTypeT>(ii), static_cast<DataTypeT>(ii * 10));
    }

    auto retval = image;
    auto endianness = std::endian::native; // "conditional expression is constant"
    if (endianness == std::endian::little)
    {
        sys::byteSwap(retval.data(), sizeof(DataTypeT), retval.size() * 2);
    }
    return retval;
}

// Main test class
template <typename DataTypeT>
struct Tester final
{
    Tester(const std::vector<std::string>& schemaPaths, bool setMaxProductSize, size_t maxProductSize = 0) :
        mNormalFileCleanup(mNormalPathname),
        mData(createData<DataTypeT>(mDims).release()),
        mImage(mDims.area()),
        mSchemaPaths(schemaPaths),
        mSetMaxProductSize(setMaxProductSize),
        mMaxProductSize(maxProductSize)
    {
        mBigEndianImage = createBigEndianImage(mImage);
        normalWrite();
    }

    // Write the file out with a SICDByteProvider in one shot
    bool testSingleWrite()
    {
        const EnsureFileCleanup ensureFileCleanup(mTestPathname);

        const NITFWriter nitfWriter(*mData, mSchemaPaths, mSetMaxProductSize ? mMaxProductSize : 0);

        constexpr size_t startRow = 0;
        const size_t numRows = mDims.row;
        mSuccess = nitfWriter.write(mTestPathname, &mBigEndianImage[startRow * mDims.col], startRow, numRows);

        compare("Single write");
        return mSuccess;
    }

private:
    void writeBytes(io::FileOutputStream& outStream, const NITFWriter& nitfWriter, size_t startRow, size_t numRows)
    {
        mSuccess = nitfWriter.write(outStream, &mBigEndianImage[startRow * mDims.col], startRow, numRows);
    }

public:
    bool testMultipleWrites()
    {
        const EnsureFileCleanup ensureFileCleanup(mTestPathname);

        const NITFWriter nitfWriter(*mData, mSchemaPaths, mSetMaxProductSize ? mMaxProductSize : 0);
        io::FileOutputStream outStream(mTestPathname);

        writeBytes(outStream, nitfWriter, 40, 20); // Rows [40, 60)
        if (mSuccess)
        {
            writeBytes(outStream, nitfWriter, 5, 20);  // Rows [5, 25)
            if (mSuccess)
            {
                writeBytes(outStream, nitfWriter, 0, 5);  // Rows [0, 5)
                if (mSuccess)
                {
                    writeBytes(outStream, nitfWriter, 100, 23);  // Rows [100, 123)        
                    if (mSuccess)
                    {
                        writeBytes(outStream, nitfWriter, 25, 15); // Rows [25, 40)       
                        if (mSuccess)
                        {
                            writeBytes(outStream, nitfWriter, 60, 40);  // Rows [60, 100)
                        }
                    }
                }
            }
        }
        outStream.close();

        compare("Multiple writes");
        return mSuccess;
    }

    bool testOneWritePerRow()
    {
        const EnsureFileCleanup ensureFileCleanup(mTestPathname);

        const NITFWriter nitfWriter(*mData, mSchemaPaths, mSetMaxProductSize ? mMaxProductSize : 0);

        io::FileOutputStream outStream(mTestPathname);
        for (size_t row = 0; row < mDims.row; ++row)
        {
            // Write it backwards
            const size_t startRow = mDims.row - 1 - row;
            constexpr size_t numRows = 1;
            writeBytes(outStream, nitfWriter, startRow, numRows);
            if (!mSuccess) break;
        }
        outStream.close();

        compare("One write per row");
        return mSuccess;
    }

private:
    void normalWrite()
    {
        six::Options options;
        if (mSetMaxProductSize)
        {
            options.setParameter(six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE, mMaxProductSize);
        }

        NITFWriter writer(*mData, options, mSchemaPaths);
        writer.save(mImage, mNormalPathname);

        mCompareFiles.reset(new CompareFiles(mNormalPathname));
    }

    void compare(const std::string& prefix)
    {
        std::string fullPrefix = prefix;
        if (mSetMaxProductSize)
        {
            fullPrefix += " (max product size " + std::to_string(mMaxProductSize) + ")";
        }

        mSuccess = (*mCompareFiles)(fullPrefix, mTestPathname);
    }

private:
    const std::string mNormalPathname = "normal_write.nitf";
    const EnsureFileCleanup mNormalFileCleanup;

    const types::RowCol<size_t> mDims{ 123, 456 };
    std::unique_ptr<six::sicd::ComplexData> mData;
    std::vector<std::complex<DataTypeT> > mImage;
    std::vector<std::complex<DataTypeT> > mBigEndianImage;

    std::unique_ptr<const CompareFiles> mCompareFiles;
    const std::string mTestPathname = "streaming_write.nitf";
    const std::vector<std::string> mSchemaPaths;

    bool mSetMaxProductSize;
    size_t mMaxProductSize;

    bool mSuccess = true;
};

template <typename DataTypeT>
bool doTests(const std::vector<std::string>& schemaPaths, bool setMaxProductSize, size_t numRowsPerSeg)
{
    // TODO: This math isn't quite right
    //       We also end up with a different number of segments for the complex float than the complex short case sometimes
    //       It would be better to get the logic fixed that forces segmentation on the number of rows via OPT_MAX_ILOC_ROWS
    constexpr size_t APPROX_HEADER_SIZE = 2 * 1024;
    constexpr size_t numBytesPerRow = 456 * sizeof(std::complex<DataTypeT>);
    const size_t maxProductSize = numRowsPerSeg * numBytesPerRow + APPROX_HEADER_SIZE;

    Tester<DataTypeT> tester(schemaPaths, setMaxProductSize, maxProductSize);
    auto result = tester.testSingleWrite();
    if (!result) return result;
    result = tester.testMultipleWrites();
    if (!result) return result;
    return tester.testOneWritePerRow();
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

// Test program for SICDByteProvider
// Demonstrates that the raw bytes provided by this class result in equivalent
// SICDs to the normal writes via NITFWriteControl
TEST_CASE(sicd_byte_provider)
{
    const std::vector<std::string> schemaPaths;

    // Run tests with no funky segmentation
    auto result = doTests<float>(schemaPaths, false /*setMaxProductSize*/, 0 /*numRowsPerSeg*/);
    TEST_ASSERT_TRUE(result);
    result = doTests<int16_t>(schemaPaths, false /*setMaxProductSize*/, 0 /*numRowsPerSeg*/);
    TEST_ASSERT_TRUE(result);

    // Run tests forcing various numbers of segments
    const std::vector<size_t> numRows{ 80, 30, 15, 7, 3, 2, 1 };
    for (auto row : numRows)
    {
        result = doTests<float>(schemaPaths, true /*setMaxProductSize*/, row);
        TEST_ASSERT_TRUE(result);
        result = doTests<int16_t>(schemaPaths, true /*setMaxProductSize*/, row);
        TEST_ASSERT_TRUE(result);
    }
}

class CompressFloat final
{
    static float round(float num)
    {
        float f = num - std::floor(num);
        return (f > 0.5f) ? std::ceil(num) : std::floor(num);
    }
public:
    CompressFloat(float min, float diff) : mMin(min), mDiff(diff) { }

    inline int16_t operator()(float value) const
    {
        const float ret(round((((value - mMin) / mDiff) *
            std::numeric_limits<uint16_t>::max()) +
            std::numeric_limits<int16_t>::min()));
        return static_cast<int16_t>(ret);
    }

private:
    const float mMin;
    const float mDiff;
};

struct Buffers final
{
    std::byte* add(size_t numBytes)
    {
        mBuffers.push_back(std::unique_ptr<std::byte[]>(new std::byte[numBytes]));
        return mBuffers.back().get();
    }

    std::vector<std::byte*> get() const
    {
        std::vector<std::byte*> retval;
        for (auto& buffer : mBuffers)
        {
            retval.push_back(buffer.get());
        }
        return retval;
    }

private:
    std::vector<std::unique_ptr<std::byte[]>> mBuffers;
};

// We've stored the complex<short> in the second half of the buffer
// We'll expand to complex<float> starting in the first half of the buffer
void expandComplex(size_t numPixels, std::byte* buffer)
{
    const std::complex<short>* const input =
        reinterpret_cast<std::complex<short>*>(buffer + numPixels * sizeof(std::complex<short>));
    std::complex<float>* const output = reinterpret_cast<std::complex<float>*>(buffer);

    for (size_t ii = 0; ii < numPixels; ++ii)
    {
        output[ii] = std::complex<float>(input[ii].real(), input[ii].imag());
    }
}

// Because we are compressing, it is safe to write directly on top of the
// buffer. This will end up with a buffer that is twice as big as needed
// because we go from a 32 to 16 bit value, but the "extra" data can
// simply be ignored.
//
// Since our buffer could be in the range of [-1.0f, 1.0f] we cannot simply
// cast to a 16 bit int. Instead we expand the values so they always go from
// [-32K, 32K].
void compressInteger(size_t numPixels, std::byte* buffer)
{
    const float* const floatValues = reinterpret_cast<float*>(buffer);

    // Find the min and max values of either real or imag
    float min = floatValues[0];
    float max = floatValues[0];
    for (size_t ii = 1; ii < numPixels * 2; ++ii)
    {
        if (floatValues[ii] < min)
        {
            min = floatValues[ii];
        }
        if (floatValues[ii] > max)
        {
            max = floatValues[ii];
        }
    }

    const std::complex<float>* const input = reinterpret_cast<std::complex<float>*>(buffer);

    std::complex<int16_t>* const output = reinterpret_cast<std::complex<int16_t>*>(buffer);
    const float diff = max - min;

    // If diff ends up being zero, we will get a division by 0 error.
    // This means that buffer is all the same value so we can just
    // fill it with 0s.
    if (diff == 0.0f)
    {
        std::fill_n(output, numPixels, std::complex<int16_t>(0, 0));
        return;
    }

    const CompressFloat compressFloat(min, diff);
    for (size_t ii = 0; ii < numPixels; ++ii)
    {
        output[ii] = std::complex<int16_t>(compressFloat(input[ii].real()), compressFloat(input[ii].imag()));
    }
}

TEST_CASE(sicd_read_data)
{
    const auto inputPathname = getNitfPath("sicd_50x50.nitf");

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
    auto data = getComplexData(*container, 0);
    TEST_ASSERT_EQ(six::PixelType::RE32F_IM32F, data->getPixelType());

    size_t numBytesPerPixel = data->getNumBytesPerPixel();
    TEST_ASSERT_EQ(8, numBytesPerPixel);

    const auto& classification = data->getClassification();
    TEST_ASSERT_TRUE(classification.isUnclassified());

    six::Region region;
    Buffers buffers;

    const auto extent = data->getExtent();
    const auto numPixels = extent.area();

    size_t offset = 0;
    bool expandIt = true;
    bool compressIt = true;
    if (expandIt)
    {
        // We'll make the buffer twice as large as we need for reading, read into the second half of it, then start
        // expanding into the first half of it
        offset = numPixels * numBytesPerPixel;
        numBytesPerPixel *= 2;
    }

    auto buffer = buffers.add(numPixels * numBytesPerPixel);
    region.setDims(extent);
    region.setBuffer(buffer + offset);
    reader.interleaved(region, 0);

    if (expandIt)
    {
        expandComplex(numPixels, buffer);
        data->setPixelType(six::PixelType::RE32F_IM32F);
    }
    if (compressIt)
    {
        compressInteger(numPixels, buffer);
        data->setPixelType(six::PixelType::RE16I_IM16I);
    }
}

TEST_MAIN((void)argc;
    argv0 = fs::absolute(argv[0]);
    TEST_CHECK(valid_six_50x50);
    TEST_CHECK(read_8bit_ampphs_with_table);    
    TEST_CHECK(read_8bit_ampphs_no_table);
    TEST_CHECK(sicd_byte_provider);    
    TEST_CHECK(sicd_read_data);
    )

