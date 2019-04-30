/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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

// Test program for SIDDByteProvider
// Demonstrates that the raw bytes provided by this class result in equivalent
// SIDDs to the normal writes via NITFWriteControl

#include <iostream>
#include <limits>
#include <stdlib.h>

#include <io/ReadUtils.h>
#include <math/Round.h>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>
#include <six/sidd/Utilities.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/SIDDByteProvider.h>

namespace
{
// Template specialization to get appropriate pixel type
template <typename DataTypeT>
struct GetPixelType
{
};

template <>
struct GetPixelType<sys::Uint8_T>
{
    static six::PixelType getPixelType()
    {
        return six::PixelType::MONO8I;
    }
};

template <>
struct GetPixelType<sys::Uint16_T>
{
    static six::PixelType getPixelType()
    {
        return six::PixelType::MONO16I;
    }
};

// Create dummy SIDD data
template <typename DataTypeT>
std::auto_ptr<six::sidd::DerivedData>
createData(const types::RowCol<size_t>& dims)
{
    std::auto_ptr<six::sidd::DerivedData> data =
            six::sidd::Utilities::createFakeDerivedData();
    data->setNumRows(dims.row);
    data->setNumCols(dims.col);
    data->setPixelType(GetPixelType<DataTypeT>::getPixelType());
    return data;
}

// Note that this will work because SIX is forcing the NITF date/time to match
// what's in the SIDD XML and we're writing the same SIDD XML in all our files
class CompareFiles
{
public:
    CompareFiles(const std::string& lhsPathname)
    {
        io::readFileContents(lhsPathname, mLHS);
    }

    bool operator()(const std::string& prefix,
                    const std::string& rhsPathname) const
    {
        io::readFileContents(rhsPathname, mRHS);

        if (mLHS == mRHS)
        {
            std::cout << prefix << " matches" << std::endl;
            return true;
        }
        else if (mLHS.size() != mRHS.size())
        {
            std::cerr << prefix << " DOES NOT MATCH: file sizes are "
                      << mLHS.size() << " vs. " << mRHS.size() << " bytes"
                      << std::endl;
        }
        else
        {
            size_t ii;
            for (ii = 0; ii < mLHS.size(); ++ii)
            {
                if (mLHS[ii] != mRHS[ii])
                {
                    break;
                }
            }

            std::cerr << prefix << " DOES NOT MATCH at byte " << ii
                      << std::endl;
        }

        return false;
    }

private:
    std::vector<sys::byte> mLHS;
    mutable std::vector<sys::byte> mRHS;
};

// Makes sure a file gets removed
// Both makes sure we start with a clean slate each time and that there are no
// leftover files if an exception occurs
class EnsureFileCleanup
{
public:
    EnsureFileCleanup(const std::string& pathname) :
        mPathname(pathname)
    {
        removeIfExists();
    }

    ~EnsureFileCleanup()
    {
        try
        {
            removeIfExists();
        }
        catch (...)
        {
        }
    }

private:
    void removeIfExists()
    {
        sys::OS os;
        if (os.exists(mPathname))
        {
            os.remove(mPathname);
        }
    }

private:
    const std::string mPathname;
};

struct WriteData
{
    size_t numBlocks;
    size_t startRow;
    size_t numRows;
};

// Main test class
template <typename DataTypeT>
class Tester
{
public:
    Tester(const std::vector<std::string>& schemaPaths,
           size_t numRowsPerBlock,
           size_t numColsPerBlock,
           bool setMaxProductSize,
           size_t maxProductSize = 0) :
        mNormalPathname("normal_write.nitf"),
        mNormalFileCleanup(mNormalPathname),
        mDims(123, 456),
        // Have to release() here to prevent nasty runtime error
        // with Solaris
        mData(createData<DataTypeT>(mDims).release()),
        mImage(mDims.area()),
        mTestPathname("streaming_write.nitf"),
        mSchemaPaths(schemaPaths),
        mNumRowsPerBlock(numRowsPerBlock),
        mNumColsPerBlock(numColsPerBlock),
        mSetMaxProductSize(setMaxProductSize),
        mMaxProductSize(maxProductSize),
        mSuccess(true)
    {
        srand(334);

        for (size_t ii = 0; ii < mImage.size(); ++ii)
        {
            mImage[ii] = static_cast<DataTypeT>(
                    rand() % std::numeric_limits<DataTypeT>::max());
        }

        mBigEndianImage = mImage;
        if (!sys::isBigEndianSystem())
        {
            sys::byteSwap(&mBigEndianImage[0],
                          sizeof(DataTypeT),
                          mBigEndianImage.size());
        }

        normalWrite();
    }

    bool success() const
    {
        return mSuccess;
    }

    // Write the file out with a SIDDByteProvider in one shot
    void testSingleWrite();

    void testMultipleWrites();

    void testMultipleWritesBlocked(size_t blocksPerWrite);

    void testOneWritePerRow();

private:
    void normalWrite();

private:
    std::string getSuffix() const
    {
        std::string suffix;
        if (mNumRowsPerBlock != 0 || mNumColsPerBlock != 0)
        {
            suffix += " with blocking of rows/block=" +
                    str::toString(mNumRowsPerBlock) +
                    ", cols/block=" +
                    str::toString(mNumColsPerBlock);
        }

        return suffix;
    }

    void compare(const std::string& prefix)
    {
        std::string fullPrefix = prefix;
        if (mSetMaxProductSize)
        {
            fullPrefix += " (max product size " +
                    str::toString(mMaxProductSize) + ")";
        }
        fullPrefix += getSuffix();

        if (!(*mCompareFiles)(fullPrefix, mTestPathname))
        {
            mSuccess = false;
        }
    }

    void setWriterOptions(six::Options& options)
    {
        if (mSetMaxProductSize)
        {
            options.setParameter(
                    six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE,
                    mMaxProductSize);
        }

        if (mNumRowsPerBlock != 0)
        {
            options.setParameter(
                    six::NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK,
                    mNumRowsPerBlock);
        }

        if (mNumColsPerBlock != 0)
        {
            options.setParameter(
                    six::NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK,
                    mNumColsPerBlock);
        }
    }

    void write(nitf::Off fileOffset,
               const nitf::NITFBufferList& buffers,
               nitf::Off computedNumBytes,
               io::FileOutputStream& outStream)
    {
        outStream.seek(fileOffset, io::Seekable::START);

        nitf::Off numBytes(0);
        for (size_t ii = 0; ii < buffers.mBuffers.size(); ++ii)
        {
            outStream.write(
                    static_cast<const sys::byte*>(buffers.mBuffers[ii].mData),
                    buffers.mBuffers[ii].mNumBytes);

            numBytes += buffers.mBuffers[ii].mNumBytes;
        }

        if (numBytes != computedNumBytes)
        {
            std::cerr << "Computed " << computedNumBytes
                      << " bytes but actually had " << numBytes << " bytes"
                      << std::endl;
            mSuccess = false;
        }
    }

    // Blocks if necessary
    const DataTypeT*
    getImage(const six::sidd::SIDDByteProvider& siddByteProvider,
             std::vector<DataTypeT>& blockedImage)
    {
        const DataTypeT* retImage;
        if (mNumRowsPerBlock != 0 || mNumColsPerBlock != 0)
        {
            std::auto_ptr<const nitf::ImageBlocker> imageBlocker =
                    siddByteProvider.getImageBlocker();

            const size_t numBlockedPixels =
                    imageBlocker->getNumBytesRequired(0, mDims.row, 1);
            blockedImage.resize(numBlockedPixels);

            imageBlocker->block(&mBigEndianImage[0],
                                0,
                                mDims.row,
                                &blockedImage[0]);

            retImage = &blockedImage[0];
        }
        else
        {
            retImage = &mBigEndianImage[0];
        }

        return retImage;
    }

private:
    const std::string mNormalPathname;
    const EnsureFileCleanup mNormalFileCleanup;

    const types::RowCol<size_t> mDims;
    std::auto_ptr<six::sidd::DerivedData> mData;
    std::vector<DataTypeT> mImage;
    std::vector<DataTypeT> mBigEndianImage;

    std::auto_ptr<const CompareFiles> mCompareFiles;
    const std::string mTestPathname;
    const std::vector<std::string> mSchemaPaths;

    const size_t mNumRowsPerBlock;
    const size_t mNumColsPerBlock;
    const bool mSetMaxProductSize;
    const size_t mMaxProductSize;

    bool mSuccess;
};

template <typename DataTypeT>
void Tester<DataTypeT>::normalWrite()
{
    mem::SharedPtr<six::Container> container(
            new six::Container(six::DataType::DERIVED));
    container->addData(mData->clone());

    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::DERIVED,
                           new six::XMLControlCreatorT<
                                   six::sidd::DerivedXMLControl>());

    six::Options options;
    setWriterOptions(options);
    six::NITFWriteControl writer(options, container, &xmlRegistry);

    six::BufferList buffers;
    buffers.push_back(reinterpret_cast<six::UByte*>(&mImage[0]));
    writer.save(buffers, mNormalPathname, mSchemaPaths);

    mCompareFiles.reset(new CompareFiles(mNormalPathname));
}

template <typename DataTypeT>
void Tester<DataTypeT>::testSingleWrite()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    const six::sidd::SIDDByteProvider siddByteProvider(
            *mData,
            mSchemaPaths,
            mNumRowsPerBlock,
            mNumColsPerBlock,
            mSetMaxProductSize ? mMaxProductSize : 0);

    std::vector<DataTypeT> scratch;
    const DataTypeT* const inImage = getImage(siddByteProvider, scratch);

    nitf::NITFBufferList buffers;
    nitf::Off fileOffset;
    siddByteProvider.getBytes(inImage, 0, mDims.row, fileOffset, buffers);
    const nitf::Off numBytes = siddByteProvider.getNumBytes(0, mDims.row);

    io::FileOutputStream outStream(mTestPathname);
    write(fileOffset, buffers, numBytes, outStream);
    outStream.close();

    compare("Single write");
}

template <typename DataTypeT>
void Tester<DataTypeT>::testMultipleWrites()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    const six::sidd::SIDDByteProvider siddByteProvider(
            *mData,
            mSchemaPaths,
            mNumRowsPerBlock,
            mNumColsPerBlock,
            mSetMaxProductSize ? mMaxProductSize : 0);

    const DataTypeT* const inImage = &mBigEndianImage[0];

    // Rows [40, 60)
    nitf::Off fileOffset;
    nitf::NITFBufferList buffers;
    size_t startRow = 40;
    size_t numRows = 20;
    siddByteProvider.getBytes(inImage + startRow * mDims.col,
                              startRow,
                              numRows,
                              fileOffset,
                              buffers);
    nitf::Off numBytes = siddByteProvider.getNumBytes(startRow, numRows);

    io::FileOutputStream outStream(mTestPathname);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [5, 25)
    startRow = 5;
    numRows = 20;
    siddByteProvider.getBytes(inImage + startRow * mDims.col,
                              startRow,
                              numRows,
                              fileOffset,
                              buffers);
    numBytes = siddByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [0, 5)
    startRow = 0;
    numRows = 5;
    siddByteProvider.getBytes(inImage + startRow * mDims.col,
                              startRow,
                              numRows,
                              fileOffset,
                              buffers);
    numBytes = siddByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [100, 123)
    startRow = 100;
    numRows = 23;
    siddByteProvider.getBytes(inImage + startRow * mDims.col,
                              startRow,
                              numRows,
                              fileOffset,
                              buffers);
    numBytes = siddByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [25, 40)
    startRow = 25;
    numRows = 15;
    siddByteProvider.getBytes(inImage + startRow * mDims.col,
                              startRow,
                              numRows,
                              fileOffset,
                              buffers);
    numBytes = siddByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    // Rows [60, 100)
    startRow = 60;
    numRows = 40;
    siddByteProvider.getBytes(inImage + startRow * mDims.col,
                              startRow,
                              numRows,
                              fileOffset,
                              buffers);
    numBytes = siddByteProvider.getNumBytes(startRow, numRows);
    write(fileOffset, buffers, numBytes, outStream);

    outStream.close();

    compare("Multiple writes");
}

size_t countNumRows(const nitf::ImageBlocker& imageBlocker,
                    size_t startBlock,
                    size_t numBlocks,
                    size_t& currentSegment,
                    size_t& startBlockThisSegment,
                    size_t& numBlocksThisSegment)
{
    size_t numRows = 0;
    size_t lastBlockThisSegment = startBlockThisSegment + numBlocksThisSegment - 1;
    for (size_t block = startBlock; block < startBlock + numBlocks; ++block)
    {
        const size_t segment = imageBlocker.getSegmentFromGlobalBlockRow(block);
        numRows += imageBlocker.getNumRowsPerBlock()[segment];

        if (segment > currentSegment)
        {
            startBlockThisSegment += numBlocksThisSegment;
            currentSegment = segment;
            numBlocksThisSegment = imageBlocker.getNumRowsOfBlocks(currentSegment);
            lastBlockThisSegment = startBlockThisSegment + numBlocksThisSegment - 1;
        }

        if (block == lastBlockThisSegment)
        {
            numRows -= imageBlocker.getNumPadRowsInFinalBlock(segment);
        }

    }
    return numRows;
}

template <typename DataTypeT>
void Tester<DataTypeT>::testMultipleWritesBlocked(size_t blocksPerWrite)
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    io::FileOutputStream outStream(mTestPathname);

    const six::sidd::SIDDByteProvider siddByteProvider(
            *mData,
            mSchemaPaths,
            mNumRowsPerBlock,
            mNumColsPerBlock,
            mSetMaxProductSize ? mMaxProductSize : 0);

    // Write the blocks in reverse order
    std::auto_ptr<const nitf::ImageBlocker> imageBlocker =
            siddByteProvider.getImageBlocker();

    const size_t numSegs(imageBlocker->getNumSegments());
    size_t totalNumBlocks(0);
    for (size_t seg = 0; seg < numSegs; ++seg)
    {
        totalNumBlocks += imageBlocker->getNumRowsOfBlocks(seg);
    }

    size_t currentSegment = 0;
    size_t numBlocksThisSeg = imageBlocker->getNumRowsOfBlocks(currentSegment);
    size_t startBlockThisSeg = 0;
    size_t startRow = 0;

    // Want to write out the blocks in reverse order to make sure offsetting
    // works. But I don't want to do the math backwards, so this will just go
    // through and collect the data needed for each block write.
    std::vector<WriteData> data;
    for (size_t startBlock = 0;
            startBlock < totalNumBlocks;
            startBlock += blocksPerWrite)
    {
        WriteData writeData;
        if (startBlock >= startBlockThisSeg + numBlocksThisSeg)
        {
            ++currentSegment;
            startBlockThisSeg += numBlocksThisSeg;
            numBlocksThisSeg = imageBlocker->getNumRowsOfBlocks(currentSegment);
        }
        const size_t blocksThisWrite =
                std::min<size_t>(blocksPerWrite, totalNumBlocks - startBlock);

        const size_t numRows = countNumRows(*imageBlocker, startBlock, blocksThisWrite,
                currentSegment, startBlockThisSeg, numBlocksThisSeg);

        writeData.numBlocks = blocksThisWrite;
        writeData.startRow = startRow;
        writeData.numRows = numRows;
        data.push_back(writeData);

        startRow += numRows;
    }

    // Solaris doesn't know how to get a const_reverse_iterator from a
    // reverse_iterator.
    const std::vector<WriteData>& constData = data;

    for (std::vector<WriteData>::const_reverse_iterator iter = constData.rbegin();
            iter != constData.rend(); ++iter)
    {
        const size_t bytesThisWrite =
            imageBlocker->getNumBytesRequired<DataTypeT>(
                    iter->startRow, iter->numRows);

        std::vector<DataTypeT> blockData(bytesThisWrite);
        imageBlocker->block(&mBigEndianImage[iter->startRow * 456],
                            iter->startRow,
                            iter->numRows,
                            &blockData[0]);

        nitf::Off fileOffset;
        nitf::NITFBufferList buffers;
        siddByteProvider.getBytes(&blockData[0],
                                  iter->startRow,
                                  iter->numRows,
                                  fileOffset,
                                  buffers);
        const size_t numBytes = siddByteProvider.getNumBytes(iter->startRow,
                                                             iter->numRows);
        write(fileOffset, buffers, numBytes, outStream);
    }
    outStream.close();

    compare("Multiple writes blocked");
}

template <typename DataTypeT>
void Tester<DataTypeT>::testOneWritePerRow()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::sidd::SIDDByteProvider siddByteProvider(
            *mData,
            mSchemaPaths,
            mNumRowsPerBlock,
            mNumColsPerBlock,
            mSetMaxProductSize ? mMaxProductSize : 0);

    io::FileOutputStream outStream(mTestPathname);
    for (size_t row = 0; row < mDims.row; ++row)
    {
        // Write it backwards
        const size_t startRow = mDims.row - 1 - row;

        nitf::Off fileOffset;
        nitf::NITFBufferList buffers;
        siddByteProvider.getBytes(&mBigEndianImage[startRow * mDims.col],
                                  startRow,
                                  1,
                                  fileOffset,
                                  buffers);
        const nitf::Off numBytes = siddByteProvider.getNumBytes(startRow, 1);
        write(fileOffset, buffers, numBytes, outStream);
    }

    outStream.close();

    compare("One write per row");
}

template <typename DataTypeT>
bool doTests(const std::vector<std::string>& schemaPaths,
             bool setBlocking,
             bool setMaxProductSize,
             size_t numRowsPerSeg)
{
    // TODO: This math isn't quite right
    //       We also end up with a different number of segments for the
    //       uint8 than the uint16 case sometimes
    //       It would be better to get the logic fixed that forces
    //       segmentation on the number of rows via OPT_MAX_ILOC_ROWS
    static const size_t APPROX_HEADER_SIZE = 2 * 1024;
    const size_t numBytesPerRow = 456 * sizeof(DataTypeT);
    const size_t maxProductSize = numRowsPerSeg * numBytesPerRow +
            APPROX_HEADER_SIZE;

    size_t totalNumBlocks(0);
    size_t numRowsPerBlock(0);
    size_t numColsPerBlock(0);
    if (setBlocking)
    {
        // These intentionally do not divide evenly so there will be both pad
        // rows and cols
        numRowsPerBlock = 7;
        numColsPerBlock = 9;
        totalNumBlocks = math::ceilingDivide(123, numRowsPerBlock);

    }

    Tester<DataTypeT> tester(schemaPaths,
                             numRowsPerBlock,
                             numColsPerBlock,
                             setMaxProductSize,
                             maxProductSize);
    tester.testSingleWrite();

    if (setBlocking)
    {
        for (size_t ii = 1; ii <= totalNumBlocks; ++ii)
        {
            tester.testMultipleWritesBlocked(ii);
        }
    }
    else
    {
        tester.testMultipleWrites();
        tester.testOneWritePerRow();
    }

    return tester.success();
}

bool doTestsBothDataTypes(const std::vector<std::string>& schemaPaths,
                          bool setMaxProductSize,
                          size_t numRowsPerSeg = 0)
{
    bool success = true;
    if (!doTests<sys::Uint8_T>(schemaPaths, false,
                               setMaxProductSize, numRowsPerSeg))
    {
        success = false;
    }

    if (!doTests<sys::Uint8_T>(schemaPaths, true,
                               setMaxProductSize, numRowsPerSeg))
    {
        success = false;
    }

    if (!doTests<sys::Uint16_T>(schemaPaths, false,
                                setMaxProductSize, numRowsPerSeg))
    {
        success = false;
    }

    if (!doTests<sys::Uint16_T>(schemaPaths, true,
                                setMaxProductSize, numRowsPerSeg))
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
        // Blocking is set at 7 rows / block so can't go less than this
        std::vector<size_t> numRows;
        numRows.push_back(80);
        numRows.push_back(30);
        numRows.push_back(15);
        numRows.push_back(7);

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
