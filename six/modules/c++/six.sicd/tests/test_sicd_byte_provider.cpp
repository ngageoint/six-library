/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

// Test program for SICDByteProvider
// Demonstrates that the raw bytes provided by this class result in equivalent
// SICDs to the normal writes via NITFWriteControl

#include <iostream>

#include "TestUtilities.h"

#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/SICDByteProvider.h>

namespace
{
// Main test class
template <typename DataTypeT>
class Tester
{
public:
    Tester(const std::vector<std::string>& schemaPaths,
           bool setMaxProductSize,
           size_t maxProductSize = 0) :
        mNormalPathname("normal_write.nitf"),
        mNormalFileCleanup(mNormalPathname),
        mDims(123, 456),
        mData(createData<DataTypeT>(mDims)),
        mImage(mDims.area()),
        mTestPathname("streaming_write.nitf"),
        mSchemaPaths(schemaPaths),
        mSetMaxProductSize(setMaxProductSize),
        mMaxProductSize(maxProductSize),
        mSuccess(true)
    {
        for (size_t ii = 0; ii < mImage.size(); ++ii)
        {
            mImage[ii] = std::complex<DataTypeT>(
                    static_cast<DataTypeT>(ii),
                    static_cast<DataTypeT>(ii * 10));
        }

        mBigEndianImage = mImage;
        if (!sys::isBigEndianSystem())
        {
            sys::byteSwap(&mBigEndianImage[0],
                          sizeof(DataTypeT),
                          mBigEndianImage.size() * 2);
        }

        normalWrite();
    }

    bool success() const
    {
        return mSuccess;
    }

    // Write the file out with a SICDByteProvider in one shot
    void testSingleWrite();

    void testMultipleWrites();

    void testOneWritePerRow();

private:
    void normalWrite();

private:
    void compare(const std::string& prefix)
    {
        std::string fullPrefix = prefix;
        if (mSetMaxProductSize)
        {
            fullPrefix += " (max product size " +
                    str::toString(mMaxProductSize) + ")";
        }

        if (!(*mCompareFiles)(fullPrefix, mTestPathname))
        {
            mSuccess = false;
        }
    }

    void setMaxProductSize(six::NITFWriteControl& writer)
    {
        if (mSetMaxProductSize)
        {
            writer.getOptions().setParameter(
                    six::NITFWriteControl::OPT_MAX_PRODUCT_SIZE, mMaxProductSize);
        }
    }

    static
    void write(nitf::Off fileOffset,
               const six::sicd::SICDByteProvider::BufferList& buffers,
               io::FileOutputStream& outStream)
    {
        outStream.seek(fileOffset, io::Seekable::START);

        for (size_t ii = 0; ii < buffers.mBuffers.size(); ++ii)
        {
            outStream.write(
                    static_cast<const sys::byte*>(buffers.mBuffers[ii].mData),
                    buffers.mBuffers[ii].mNumBytes);
        }
    }

private:
    const std::string mNormalPathname;
    const EnsureFileCleanup mNormalFileCleanup;

    const types::RowCol<size_t> mDims;
    std::auto_ptr<six::sicd::ComplexData> mData;
    std::vector<std::complex<DataTypeT> > mImage;
    std::vector<std::complex<DataTypeT> > mBigEndianImage;

    std::auto_ptr<const CompareFiles> mCompareFiles;
    const std::string mTestPathname;
    const std::vector<std::string> mSchemaPaths;

    bool mSetMaxProductSize;
    size_t mMaxProductSize;

    bool mSuccess;
};

template <typename DataTypeT>
void Tester<DataTypeT>::normalWrite()
{
    mem::SharedPtr<six::Container> container(
            new six::Container(six::DataType::COMPLEX));
    container->addData(mData->clone());

    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
                           new six::XMLControlCreatorT<
                                   six::sicd::ComplexXMLControl>());

    six::NITFWriteControl writer;
    writer.setXMLControlRegistry(&xmlRegistry);
    setMaxProductSize(writer);
    writer.initialize(container);

    six::BufferList buffers;
    buffers.push_back(reinterpret_cast<six::UByte*>(&mImage[0]));
    writer.save(buffers, mNormalPathname, mSchemaPaths);

    mCompareFiles.reset(new CompareFiles(mNormalPathname));
}

template <typename DataTypeT>
void Tester<DataTypeT>::testSingleWrite()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::sicd::SICDByteProvider sicdByteProvider(
            *mData,
            mSchemaPaths,
            mSetMaxProductSize ? mMaxProductSize : 0);

    six::sicd::SICDByteProvider::BufferList buffers;
    nitf::Off fileOffset;
    sicdByteProvider.getBytes(&mBigEndianImage[0], 0, mDims.row,
                              fileOffset, buffers);

    io::FileOutputStream outStream(mTestPathname);
    write(fileOffset, buffers, outStream);
    outStream.close();

    compare("Single write");
}

template <typename DataTypeT>
void Tester<DataTypeT>::testMultipleWrites()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::sicd::SICDByteProvider sicdByteProvider(
            *mData,
            mSchemaPaths,
            mSetMaxProductSize ? mMaxProductSize : 0);

    // Rows [40, 60)
    nitf::Off fileOffset;
    six::sicd::SICDByteProvider::BufferList buffers;
    size_t startRow = 40;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col],
                              startRow,
                              20,
                              fileOffset,
                              buffers);

    io::FileOutputStream outStream(mTestPathname);
    write(fileOffset, buffers, outStream);

    // Rows [5, 25)
    startRow = 5;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col],
                              startRow,
                              20,
                              fileOffset,
                              buffers);
    write(fileOffset, buffers, outStream);

    // Rows [0, 5)
    startRow = 0;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col],
                              startRow,
                              5,
                              fileOffset,
                              buffers);
    write(fileOffset, buffers, outStream);

    // Rows [100, 123)
    startRow = 100;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col],
                              startRow,
                              23,
                              fileOffset,
                              buffers);
    write(fileOffset, buffers, outStream);

    // Rows [25, 40)
    startRow = 25;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col],
                              startRow,
                              15,
                              fileOffset,
                              buffers);
    write(fileOffset, buffers, outStream);

    // Rows [60, 100)
    startRow = 60;
    sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col],
                              startRow,
                              40,
                              fileOffset,
                              buffers);
    write(fileOffset, buffers, outStream);

    outStream.close();

    compare("Multiple writes");
}

template <typename DataTypeT>
void Tester<DataTypeT>::testOneWritePerRow()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::sicd::SICDByteProvider sicdByteProvider(
            *mData,
            mSchemaPaths,
            mSetMaxProductSize ? mMaxProductSize : 0);

    io::FileOutputStream outStream(mTestPathname);
    for (size_t row = 0; row < mDims.row; ++row)
    {
        // Write it backwards
        const size_t startRow = mDims.row - 1 - row;

        nitf::Off fileOffset;
        six::sicd::SICDByteProvider::BufferList buffers;
        sicdByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col],
                                  startRow,
                                  1,
                                  fileOffset,
                                  buffers);
        write(fileOffset, buffers, outStream);
    }

    outStream.close();

    compare("One write per row");
}

template <typename DataTypeT>
bool doTests(const std::vector<std::string>& schemaPaths,
             bool setMaxProductSize,
             size_t numRowsPerSeg)
{
    // TODO: This math isn't quite right
    //       We also end up with a different number of segments for the
    //       complex float than the complex short case sometimes
    //       It would be better to get the logic fixed that forces
    //       segmentation on the number of rows via OPT_MAX_ILOC_ROWS
    static const size_t APPROX_HEADER_SIZE = 2 * 1024;
    const size_t numBytesPerRow = 456 * sizeof(std::complex<DataTypeT>);
    const size_t maxProductSize = numRowsPerSeg * numBytesPerRow +
            APPROX_HEADER_SIZE;

    Tester<DataTypeT> tester(schemaPaths, setMaxProductSize, maxProductSize);
    tester.testSingleWrite();
    tester.testMultipleWrites();
    tester.testOneWritePerRow();

    return tester.success();
}

bool doTestsBothDataTypes(const std::vector<std::string>& schemaPaths,
                          bool setMaxProductSize,
                          size_t numRowsPerSeg = 0)
{
    bool success = true;
    if (!doTests<float>(schemaPaths, setMaxProductSize, numRowsPerSeg))
    {
        success = false;
    }

    if (!doTests<sys::Int16_T>(schemaPaths, setMaxProductSize, numRowsPerSeg))
    {
        success = false;
    }

    return success;
}
}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        // TODO: Take these in optionally
        const std::vector<std::string> schemaPaths;

        // Run tests with no funky segmentation
        bool success = true;
        if (!doTestsBothDataTypes(schemaPaths, false))
        {
            success = false;
        }

        // Run tests forcing various numbers of segments
        std::vector<size_t> numRows;
        numRows.push_back(80);
        numRows.push_back(30);
        numRows.push_back(15);
        numRows.push_back(7);
        numRows.push_back(3);
        numRows.push_back(2);
        numRows.push_back(1);

        for (size_t ii = 0; ii < numRows.size(); ++ii)
        {
            if (!doTestsBothDataTypes(schemaPaths, true, numRows[ii]))
            {
                success = false;
            }
        }

        // With any luck we passed
        if (success)
        {
            std::cout << "All tests pass!\n";
        }
        else
        {
            std::cerr << "Some tests FAIL!\n";
        }

        return (success ? 0 : 1);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::Exception: " << ex.getMessage()
                  << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }
}
