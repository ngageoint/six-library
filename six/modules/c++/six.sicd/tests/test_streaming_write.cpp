/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

// Test program for SICDWriteControl
// Demonstrates that streaming writes result in equivalent SICDs to the normal
// writes via NITFWriteControl

#include <iostream>

#include "TestUtilities.h"

#include <import/six.h>
#include <import/io.h>
#include <logging/Setup.h>
#include <scene/Utilities.h>

#include <import/six/sicd.h>
#include <six/sicd/SICDWriteControl.h>

namespace
{
// Grab an AOI out of 'orig' specified by 'offset' and 'dims'
template <typename T>
void subsetData(const T* orig,
                size_t origNumCols,
                const types::RowCol<size_t>& offset,
                const types::RowCol<size_t>& dims,
                std::vector<T>& output)
{
    output.resize(dims.area());
    const T* origPtr = orig + offset.row * origNumCols + offset.col;
    T* outputPtr = &output[0];
    for (size_t row = 0;
         row < dims.row;
         ++row, origPtr += origNumCols, outputPtr += dims.col)
    {
        ::memcpy(outputPtr, origPtr, dims.col * sizeof(T));
    }
}

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
        mContainer(new six::Container(six::DataType::COMPLEX)),
        mDims(123, 456),
        mImage(mDims.area()),
        mImagePtr(&mImage[0]),
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

        normalWrite();
    }

    bool success() const
    {
        return mSuccess;
    }

    // Write the file out with a SICDWriteControl in one shot
    void testSingleWrite();

    // Writes with num cols == global num cols
    void testMultipleWritesOfFullRows();

    // Writes where some rows are written out with only some of the cols
    void testMultipleWritesOfPartialRows();

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

    void setMaxProductSize(six::Options& options)
    {
        if (mSetMaxProductSize)
        {
            options.setParameter(
                    six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE, mMaxProductSize);
        }
    }

private:
    const std::string mNormalPathname;
    const EnsureFileCleanup mNormalFileCleanup;

    mem::SharedPtr<six::Container> mContainer;
    const types::RowCol<size_t> mDims;
    std::vector<std::complex<DataTypeT> > mImage;
    std::complex<DataTypeT>* const mImagePtr;

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
    mContainer->addData(createData<DataTypeT>(mDims).release());

    six::Options options;
    setMaxProductSize(options);
    six::NITFWriteControl writer(options, mContainer);

    six::BufferList buffers;
    buffers.push_back(reinterpret_cast<six::UByte*>(mImagePtr));
    writer.save(buffers, mNormalPathname, mSchemaPaths);

    mCompareFiles.reset(new CompareFiles(mNormalPathname));
}

template <typename DataTypeT>
void Tester<DataTypeT>::testSingleWrite()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::Options options;
    setMaxProductSize(options);
    six::sicd::SICDWriteControl sicdWriter(mTestPathname, mSchemaPaths);
    sicdWriter.initialize(options, mContainer);

    sicdWriter.save(mImagePtr, types::RowCol<size_t>(0, 0), mDims);
    sicdWriter.close();

    compare("Single write");
}

template <typename DataTypeT>
void Tester<DataTypeT>::testMultipleWritesOfFullRows()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::Options options;
    setMaxProductSize(options);
    six::sicd::SICDWriteControl sicdWriter(mTestPathname, mSchemaPaths);
    sicdWriter.initialize(options, mContainer);

    // Rows [40, 60)
    types::RowCol<size_t> offset(40, 0);
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(20, mDims.col));

    // Rows [5, 25)
    offset.row = 5;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(20, mDims.col));

    // Rows [0, 5)
    offset.row = 0;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(5, mDims.col));

    // Rows [100, 123)
    offset.row = 100;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(23, mDims.col));

    // Rows [25, 40)
    offset.row = 25;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(15, mDims.col));

    // Rows [60, 100)
    offset.row = 60;
    sicdWriter.save(mImagePtr + offset.row * mDims.col,
                    offset,
                    types::RowCol<size_t>(40, mDims.col));

    sicdWriter.close();

    compare("Multiple writes of full rows");
}

template <typename DataTypeT>
void Tester<DataTypeT>::testMultipleWritesOfPartialRows()
{
    const EnsureFileCleanup ensureFileCleanup(mTestPathname);

    six::Options options;
    setMaxProductSize(options);

    six::sicd::SICDWriteControl sicdWriter(mTestPathname, mSchemaPaths);
    sicdWriter.initialize(options, mContainer);

    // Rows [40, 60)
    // Cols [400, 456)
    types::RowCol<size_t> offset(40, 400);
    std::vector<std::complex<DataTypeT> > subset;
    types::RowCol<size_t> subsetDims(20, 56);
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    // Rows [60, 123)
    offset.row = 60;
    offset.col = 0;
    subsetDims.row = 63;
    subsetDims.col = mDims.col;
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    // Rows [40, 60)
    // Cols [150, 400)
    offset.row = 40;
    offset.col = 150;
    subsetDims.row = 20;
    subsetDims.col = 250;
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    // Rows [0, 40)
    offset.row = 0;
    offset.col = 0;
    subsetDims.row = 40;
    subsetDims.col = mDims.col;
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    // Rows [40, 60)
    // Cols [0, 150)
    offset.row = 40;
    offset.col = 0;
    subsetDims.row = 20;
    subsetDims.col = 150;
    subsetData(mImagePtr, mDims.col, offset, subsetDims, subset);
    sicdWriter.save(&subset[0], offset, subsetDims);

    sicdWriter.close();

    compare("Multiple writes of partial rows");
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
    tester.testMultipleWritesOfFullRows();
    tester.testMultipleWritesOfPartialRows();

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

        // create an XML registry
        six::XMLControlFactory::getInstance().addCreator(
                six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

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
