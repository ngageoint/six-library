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
/*!
 *  This function converts DMS corners into decimal degrees using NITRO,
 *  and then puts them into a lat-lon
 */
inline
six::LatLonCorners makeUpCornersFromDMS()
{
    int latTopDMS[3] = { 42, 17, 50 };
    int latBottomDMS[3] = { 42, 15, 14 };
    int lonEastDMS[3] = { -83, 42, 12 };
    int lonWestDMS[3] = { -83, 45, 44 };

    const double latTopDecimal =
        nitf::Utils::geographicToDecimal(latTopDMS[0],
                                         latTopDMS[1],
                                         latTopDMS[2]);

    const double latBottomDecimal =
        nitf::Utils::geographicToDecimal(latBottomDMS[0],
                                         latBottomDMS[1],
                                         latBottomDMS[2]);

    const double lonEastDecimal =
        nitf::Utils::geographicToDecimal(lonEastDMS[0],
                                         lonEastDMS[1],
                                         lonEastDMS[2]);

    const double lonWestDecimal =
        nitf::Utils::geographicToDecimal(lonWestDMS[0],
                                         lonWestDMS[1],
                                         lonWestDMS[2]);

    six::LatLonCorners corners;
    corners.upperLeft = six::LatLon(latTopDecimal, lonWestDecimal);
    corners.upperRight = six::LatLon(latTopDecimal, lonEastDecimal);
    corners.lowerRight = six::LatLon(latBottomDecimal, lonEastDecimal);
    corners.lowerLeft = six::LatLon(latBottomDecimal, lonWestDecimal);
    return corners;
}

template <typename DataTypeT>
struct GetPixelType
{
};

template <>
struct GetPixelType<float>
{
    static six::PixelType getPixelType()
    {
        return six::PixelType::RE32F_IM32F;
    }
};

template <>
struct GetPixelType<sys::Int16_T>
{
    static six::PixelType getPixelType()
    {
        return six::PixelType::RE16I_IM16I;
    }
};

// Create dummy SICD data
template <typename DataTypeT>
std::unique_ptr<six::Data>
createData(const types::RowCol<size_t>& dims)
{
    six::sicd::ComplexData* data(new six::sicd::ComplexData());
    std::unique_ptr<six::Data> scopedData(data);
    data->setPixelType(GetPixelType<DataTypeT>::getPixelType());
    data->setNumRows(dims.row);
    data->setNumCols(dims.col);
    data->setName("corename");
    data->setSource("sensorname");
    data->collectionInformation->setClassificationLevel("UNCLASSIFIED");
    data->setCreationTime(six::DateTime());
    data->setImageCorners(makeUpCornersFromDMS());
    data->collectionInformation->radarMode = six::RadarModeType::SPOTLIGHT;
    data->scpcoa->sideOfTrack = six::SideOfTrackType::LEFT;
    data->geoData->scp.llh = six::LatLonAlt(42.2708, -83.7264);
    data->geoData->scp.ecf =
            scene::Utilities::latLonToECEF(data->geoData->scp.llh);
    data->grid->timeCOAPoly = six::Poly2D(0, 0);
    data->grid->timeCOAPoly[0][0] = 15605743.142846;
    data->position->arpPoly = six::PolyXYZ(0);
    data->position->arpPoly[0] = 0.0;

    data->radarCollection->txFrequencyMin = 0.0;
    data->radarCollection->txFrequencyMax = 0.0;
    data->radarCollection->txPolarization = six::PolarizationType::OTHER;
    mem::ScopedCloneablePtr<six::sicd::ChannelParameters>
            rcvChannel(new six::sicd::ChannelParameters());
    rcvChannel->txRcvPolarization = six::DualPolarizationType::OTHER;
    data->radarCollection->rcvChannels.push_back(rcvChannel);

    data->grid->row->sign = six::FFTSign::POS;
    data->grid->row->unitVector = 0.0;
    data->grid->row->sampleSpacing = 0;
    data->grid->row->impulseResponseWidth = 0;
    data->grid->row->impulseResponseBandwidth = 0;
    data->grid->row->kCenter = 0;
    data->grid->row->deltaK1 = 0;
    data->grid->row->deltaK2 = 0;
    data->grid->col->sign = six::FFTSign::POS;
    data->grid->col->unitVector = 0.0;
    data->grid->col->sampleSpacing = 0;
    data->grid->col->impulseResponseWidth = 0;
    data->grid->col->impulseResponseBandwidth = 0;
    data->grid->col->kCenter = 0;
    data->grid->col->deltaK1 = 0;
    data->grid->col->deltaK2 = 0;

    data->imageFormation->rcvChannelProcessed->numChannelsProcessed = 1;
    data->imageFormation->rcvChannelProcessed->channelIndex.push_back(0);

    data->pfa.reset(new six::sicd::PFA());
    data->pfa->spatialFrequencyScaleFactorPoly = six::Poly1D(0);
    data->pfa->spatialFrequencyScaleFactorPoly[0] = 42;
    data->pfa->polarAnglePoly = six::Poly1D(0);
    data->pfa->polarAnglePoly[0] = 42;

    data->timeline->collectStart = six::DateTime();
    data->timeline->collectDuration = 1.0;
    data->imageFormation->txRcvPolarizationProc =
            six::DualPolarizationType::OTHER;
    data->imageFormation->tStartProc = 0;
    data->imageFormation->tEndProc = 0;

    data->scpcoa->scpTime = 15605743.142846;
    data->scpcoa->slantRange = 0.0;
    data->scpcoa->groundRange = 0.0;
    data->scpcoa->dopplerConeAngle = 0.0;
    data->scpcoa->grazeAngle = 0.0;
    data->scpcoa->incidenceAngle = 0.0;
    data->scpcoa->twistAngle = 0.0;
    data->scpcoa->slopeAngle = 0.0;
    data->scpcoa->azimAngle = 0.0;
    data->scpcoa->layoverAngle = 0.0;
    data->scpcoa->arpPos = 0.0;
    data->scpcoa->arpVel = 0.0;
    data->scpcoa->arpAcc = 0.0;

    data->pfa->focusPlaneNormal = 0.0;
    data->pfa->imagePlaneNormal = 0.0;
    data->pfa->polarAngleRefTime = 0.0;
    data->pfa->krg1 = 0;
    data->pfa->krg2 = 0;
    data->pfa->kaz1 = 0;
    data->pfa->kaz2 = 0;

    data->imageFormation->txFrequencyProcMin = 0;
    data->imageFormation->txFrequencyProcMax = 0;

    return scopedData;
}

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

    std::shared_ptr<six::Container> mContainer;
    const types::RowCol<size_t> mDims;
    std::vector<std::complex<DataTypeT> > mImage;
    std::complex<DataTypeT>* const mImagePtr;

    std::unique_ptr<const CompareFiles> mCompareFiles;
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
