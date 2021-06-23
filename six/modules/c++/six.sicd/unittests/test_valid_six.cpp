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

class NITFWriter final
{
    mem::SharedPtr<six::Container> container{ new six::Container(six::DataType::COMPLEX) };
    six::XMLControlRegistry xmlRegistry;

    std::unique_ptr<six::NITFWriteControl> pWriter;

public:
    NITFWriter(const six::sicd::ComplexData& data, const six::Options& options)
    {
        container->addData(data.clone());

        xmlRegistry.addCreator(six::DataType::COMPLEX, new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        pWriter = std::make_unique<six::NITFWriteControl>(options, container, &xmlRegistry);
    }

    void save(const six::buffer_list& buffers, const std::string& pathname, const std::vector<std::string>& schemaPaths)
    {
        pWriter->save(buffers, pathname, schemaPaths);
    }

    template<typename DataTypeT>
    void save(const std::vector<std::complex< DataTypeT>>& image, const std::string& pathname, const std::vector<std::string>& schemaPaths)
    {
        six::buffer_list buffers;
        buffers.push_back(reinterpret_cast<const std::byte*>(image.data()));
        save(buffers, pathname, schemaPaths);
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

    bool success = true;

    // Write the file out with a SICDByteProvider in one shot
    void testSingleWrite()
    {
        const EnsureFileCleanup ensureFileCleanup(mTestPathname);

        const six::sicd::SICDByteProvider sicdByteProvider(*mData, mSchemaPaths, mSetMaxProductSize ? mMaxProductSize : 0);

        nitf::NITFBufferList buffers;
        nitf::Off fileOffset;
        sicdByteProvider.getBytes(mBigEndianImage.data(), 0, mDims.row, fileOffset, buffers);
        const nitf::Off numBytes = sicdByteProvider.getNumBytes(0, mDims.row);

        io::FileOutputStream outStream(mTestPathname);
        write(fileOffset, buffers, numBytes, outStream);
        outStream.close();

        compare("Single write");
    }

    void testMultipleWrites();
    void testOneWritePerRow()
    {
        const EnsureFileCleanup ensureFileCleanup(mTestPathname);

        six::sicd::SICDByteProvider sicdByteProvider(*mData, mSchemaPaths, mSetMaxProductSize ? mMaxProductSize : 0);

        io::FileOutputStream outStream(mTestPathname);
        for (size_t row = 0; row < mDims.row; ++row)
        {
            // Write it backwards
            const size_t startRow = mDims.row - 1 - row;

            nitf::Off fileOffset;
            nitf::NITFBufferList buffers;
            sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col], startRow, 1, fileOffset, buffers);
            const nitf::Off numBytes = sicdByteProvider.getNumBytes(startRow, 1);
            write(fileOffset, buffers, numBytes, outStream);
        }

        outStream.close();

        compare("One write per row");
    }

private:
    void normalWrite()
    {
        six::Options options;
        if (mSetMaxProductSize)
        {
            options.setParameter(six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE, mMaxProductSize);
        }

        NITFWriter writer(*mData, options);
        writer.save(mImage, mNormalPathname, mSchemaPaths);

        mCompareFiles.reset(new CompareFiles(mNormalPathname));
    }

    void compare(const std::string& prefix)
    {
        std::string fullPrefix = prefix;
        if (mSetMaxProductSize)
        {
            fullPrefix += " (max product size " + std::to_string(mMaxProductSize) + ")";
        }

        if (!(*mCompareFiles)(fullPrefix, mTestPathname))
        {
            success = false;
        }
    }

    void write(nitf::Off fileOffset, const nitf::NITFBufferList& buffers, nitf::Off computedNumBytes, io::FileOutputStream& outStream)
    {
        outStream.seek(fileOffset, io::Seekable::START);

        nitf::Off numBytes(0);
        for (const auto& buffer : buffers.mBuffers)
        {
            outStream.write(static_cast<const std::byte*>(buffer.mData), buffer.mNumBytes);
            numBytes += buffer.mNumBytes;
        }

        if (numBytes != computedNumBytes)
        {
            std::cerr << "Computed " << computedNumBytes << " bytes but actually had " << numBytes << " bytes\n";
            success = false;
        }
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
};
template <typename DataTypeT>
void Tester<DataTypeT>::testMultipleWrites()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    const six::sicd::SICDByteProvider sicdByteProvider(*mData, mSchemaPaths, mSetMaxProductSize ? mMaxProductSize : 0);

    // Rows [40, 60)
    nitf::Off fileOffset;
    nitf::NITFBufferList buffers;
    size_t startRow = 40, numRows = 20;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col], startRow, numRows, fileOffset, buffers);
    nitf::Off numBytes = sicdByteProvider.getNumBytes(startRow, numRows);

    io::FileOutputStream outStream(mTestPathname);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [5, 25)
    startRow = 5; numRows = 20;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col], startRow, numRows, fileOffset, buffers);
    numBytes = sicdByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [0, 5)
    startRow = 0; numRows = 5;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col], startRow, numRows, fileOffset, buffers);
    numBytes = sicdByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [100, 123)
    startRow = 100; numRows = 23;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col], startRow, numRows, fileOffset, buffers);
    numBytes = sicdByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [25, 40)
    startRow = 25; numRows = 15;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col], startRow, numRows, fileOffset, buffers);
    numBytes = sicdByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [60, 100)
    startRow = 60; numRows = 40;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col], startRow, numRows, fileOffset, buffers);
    numBytes = sicdByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    outStream.close();

    compare("Multiple writes");
}

template <typename DataTypeT>
bool doTests(const std::vector<std::string>& schemaPaths, bool setMaxProductSize, size_t numRowsPerSeg)
{
    // TODO: This math isn't quite right
    //       We also end up with a different number of segments for the
    //       complex float than the complex short case sometimes
    //       It would be better to get the logic fixed that forces
    //       segmentation on the number of rows via OPT_MAX_ILOC_ROWS
    constexpr size_t APPROX_HEADER_SIZE = 2 * 1024;
    constexpr size_t numBytesPerRow = 456 * sizeof(std::complex<DataTypeT>);
    const size_t maxProductSize = numRowsPerSeg * numBytesPerRow + APPROX_HEADER_SIZE;

    Tester<DataTypeT> tester(schemaPaths, setMaxProductSize, maxProductSize);
    tester.testSingleWrite();
    tester.testMultipleWrites();
    tester.testOneWritePerRow();
    return tester.success;
}

bool doTestsBothDataTypes(const std::vector<std::string>& schemaPaths, bool setMaxProductSize, size_t numRowsPerSeg = 0)
{
    if (!doTests<float>(schemaPaths, setMaxProductSize, numRowsPerSeg))
    {
        return false;
    }
    if (!doTests<int16_t>(schemaPaths, setMaxProductSize, numRowsPerSeg))
    {
        return false;
    }
    return true;
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

static std::unique_ptr<const six::sicd::ComplexData> getComplexData(const six::Container& container, size_t jj)
{
    std::unique_ptr<six::Data> data_;
    data_.reset(container.getData(jj)->clone());

    TEST_ASSERT_EQ(six::DataType::COMPLEX, data_->getDataType());
    std::unique_ptr<const six::sicd::ComplexData> retval(dynamic_cast<six::sicd::ComplexData*>(data_.release()));

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

// Test program for SICDByteProvider
// Demonstrates that the raw bytes provided by this class result in equivalent
// SICDs to the normal writes via NITFWriteControl
TEST_CASE(sicd_byte_provider)
{
    const std::vector<std::string> schemaPaths;

    // Run tests with no funky segmentation
    bool success = doTestsBothDataTypes(schemaPaths, false);
    TEST_ASSERT_TRUE(success);

    // Run tests forcing various numbers of segments
    const std::vector<size_t> numRows{ 80, 30, 15, 7, 3, 2, 1 };
    for (auto row : numRows)
    {
        success = doTestsBothDataTypes(schemaPaths, true, row);
        TEST_ASSERT_TRUE(success);
    }
}

TEST_MAIN((void)argc;
    argv0 = fs::absolute(argv[0]);
    TEST_CHECK(valid_six_50x50);
    TEST_CHECK(read_8bit_ampphs_with_table);    
    TEST_CHECK(read_8bit_ampphs_no_table);
    TEST_CHECK(sicd_byte_provider);    
    )

