/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <memory>
#include <sstream>
#include <limits>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <six/NITFImageInfo.h>
#include <scene/Utilities.h>

namespace
{
class GetDisplayLutFromData
{
public:
    GetDisplayLutFromData(six::Data& data) :
        mData(data)
    {
    }

    const six::LUT* operator()() const
    {
        return mData.getDisplayLUT();
    }

private:
    six::Data& mData;
};
}

namespace six
{
//!  File security classification system
const char NITFImageInfo::CLSY[] = "CLSY";
//!  File security codewords
const char NITFImageInfo::CODE[] = "CODE";
//!  File control and handling
const char NITFImageInfo::CTLH[] = "CTLH";
//!  File releasing instructions
const char NITFImageInfo::REL[] = "REL";
//!  File security declassification type
const char NITFImageInfo::DCTP[] = "DCTP";
//!  File security declassification date
const char NITFImageInfo::DCDT[] = "DCDT";
//!  File security declassification exemption
const char NITFImageInfo::DCXM[] = "DCXM";
//!  File security downgrade
const char NITFImageInfo::DG[] = "DG";
//!  File security downgrade date
const char NITFImageInfo::DGDT[] = "DGDT";
//!  File security classification text
const char NITFImageInfo::CLTX[] = "CLTX";
//!  File security classification Authority type
const char NITFImageInfo::CATP[] = "CATP";
//!  File security classification Authority
const char NITFImageInfo::CAUT[] = "CAUT";
//!  File security reason
const char NITFImageInfo::CRSN[] = "CRSN";
//!  File security source date
const char NITFImageInfo::SRDT[] = "SRDT";
//!  File security control number
const char NITFImageInfo::CTLN[] = "CTLN";


NITFImageInfo::NITFImageInfo(Data* data,
                             size_t maxRows,
                             sys::Uint64_T maxSize,
                             bool computeSegments,
                             size_t rowsPerBlock,
                             size_t colsPerBlock) :
    mData(data),
    mStartIndex(0),
    mNumRowsLimit(maxRows),
    mNumColsPaddedForBlocking(getActualDim(data->getNumCols(), colsPerBlock)),
    mMaxProductSize(maxSize),
    mNumRowsPerBlock(rowsPerBlock),
    mProductSize(static_cast<sys::Uint64_T>(mData->getNumBytesPerPixel()) *
                 getActualDim(mData->getNumRows(), rowsPerBlock) *
                 mNumColsPaddedForBlocking)
{
    if (maxRows > Constants::ILOC_MAX)
    {
        std::ostringstream ostr;
        ostr << "Max rows was set to " << maxRows
             << " but it cannot be greater than " << Constants::ILOC_MAX
             << " per the NITF spec";
        throw except::Exception(Ctxt(ostr.str()));
    }

    if (maxSize > Constants::IS_SIZE_MAX)
    {
        std::ostringstream ostr;
        ostr << "Max image segment size was set to " << maxSize
             << " but it cannot be greater than " << Constants::IS_SIZE_MAX
             << " per the NITF spec";
        throw except::Exception(Ctxt(ostr.str()));
    }

    if (computeSegments)
    {
        compute();
    }
}

size_t NITFImageInfo::getActualDim(size_t dim, size_t numDimsPerBlock)
{
    if (numDimsPerBlock == 0)
    {
        return dim;
    }
    else
    {
        // If we're blocking then we'll always write full blocks due to how
        // the NITF file layout works
        const size_t numBlocks =
                (dim / numDimsPerBlock) + (dim % numDimsPerBlock != 0);
        return numBlocks * numDimsPerBlock;
    }
}

void NITFImageInfo::computeImageInfo()
{
    // Consider possible blocking when determining the maximum number of rows
    const sys::Uint64_T bytesPerRow =
            static_cast<sys::Uint64_T>(mData->getNumBytesPerPixel()) *
            mNumColsPaddedForBlocking;

    const sys::Uint64_T maxRowsUint64 =
            static_cast<sys::Uint64_T>(mMaxProductSize) / bytesPerRow;
    if (maxRowsUint64 > std::numeric_limits<size_t>::max())
    {
        // This should not be possible
        std::ostringstream ostr;
        ostr << "Product would require " << maxRowsUint64
             << " rows which is too many";
        throw except::Exception(Ctxt(ostr.str()));
    }
    size_t maxRows(static_cast<size_t>(maxRowsUint64));

    if (maxRows == 0)
    {
        std::ostringstream ostr;
        ostr << "maxProductSize [" << mMaxProductSize << "] < bytesPerRow ["
             << bytesPerRow << "]";

        throw except::Exception(Ctxt(ostr.str()));
    }

    // Truncate back to a full block for the maxRows that will actually fit
    if (mNumRowsPerBlock != 0)
    {
        const size_t numBlocksVert = maxRows / mNumRowsPerBlock;
        maxRows = numBlocksVert * mNumRowsPerBlock;
    }

    if (maxRows < mNumRowsLimit)
    {
        mNumRowsLimit = maxRows;
    }
}

void NITFImageInfo::computeSegmentInfo()
{
    // TODO: other conditions which should trigger segmentation:
    //   NCOLS >= 10E8   column direction segmentation, no numColsLimit logic in place
    //   NROWS >= 10E8   most likely would exceed maxProductSize (ncols < 100 unlikely)
    //                   and segment to mNumRowsLimit anyway
    if (mProductSize <= mMaxProductSize)
    {
        mImageSegments.resize(1);
        mImageSegments[0].numRows = mData->getNumRows();
        mImageSegments[0].firstRow = 0;
        mImageSegments[0].rowOffset = 0;
        mImageSegments[0].corners = mData->getImageCorners();
    }

    else
    {
        // NOTE: See header for why rowOffset is always set to mNumRowsLimit
        //       for image segments 1 and above
        const size_t numIS = (size_t) std::ceil(mData->getNumRows()
                / (double) mNumRowsLimit);
        mImageSegments.resize(numIS);
        mImageSegments[0].numRows = mNumRowsLimit;
        mImageSegments[0].firstRow = 0;
        mImageSegments[0].rowOffset = 0;
        size_t i;
        for (i = 1; i < numIS - 1; i++)
        {
            mImageSegments[i].numRows = mNumRowsLimit;
            mImageSegments[i].firstRow = i * mNumRowsLimit;
            mImageSegments[i].rowOffset = mNumRowsLimit;
        }

        mImageSegments[i].firstRow = i * mNumRowsLimit;
        mImageSegments[i].rowOffset = mNumRowsLimit;
        mImageSegments[i].numRows = mData->getNumRows() - (numIS - 1)
                * mNumRowsLimit;

    }

    computeSegmentCorners();
}

void NITFImageInfo::computeSegmentCorners()
{
    const LatLonCorners corners = mData->getImageCorners();

    // (0, 0)
    Vector3 icp1 = scene::Utilities::latLonToECEF(corners.upperLeft);
    // (0, N)
    Vector3 icp2 = scene::Utilities::latLonToECEF(corners.upperRight);
    // (M, N)
    Vector3 icp3 = scene::Utilities::latLonToECEF(corners.lowerRight);
    // (M, 0)
    Vector3 icp4 = scene::Utilities::latLonToECEF(corners.lowerLeft);

    size_t numIS = mImageSegments.size();
    double total = mData->getNumRows() - 1.0;

    Vector3 ecef;
    size_t i;
    for (i = 0; i < numIS; i++)
    {
        size_t firstRow = mImageSegments[i].firstRow;
        double wgt1 = (total - firstRow) / total;
        double wgt2 = firstRow / total;

        // This requires an operator overload for scalar * vector
        ecef = wgt1 * icp1 + wgt2 * icp4;

        mImageSegments[i].corners.upperLeft =
            scene::Utilities::ecefToLatLon(ecef);

        // Now do it for the first
        ecef = wgt1 * icp2 + wgt2 * icp3;

        mImageSegments[i].corners.upperRight =
            scene::Utilities::ecefToLatLon(ecef);
    }

    for (i = 0; i < numIS - 1; i++)
    {
        LatLonCorners& theseCorners(mImageSegments[i].corners);
        const LatLonCorners& nextCorners(mImageSegments[i + 1].corners);

        theseCorners.lowerRight.setLat(nextCorners.upperRight.getLat());
        theseCorners.lowerRight.setLon(nextCorners.upperRight.getLon());

        theseCorners.lowerLeft.setLat(nextCorners.upperLeft.getLat());
        theseCorners.lowerLeft.setLon(nextCorners.upperLeft.getLon());
    }

    // This last one is cake
    mImageSegments[i].corners.lowerRight = corners.lowerRight;
    mImageSegments[i].corners.lowerLeft = corners.lowerLeft;

    // SHOULD WE JUST ASSUME THAT WHATEVER IS IN THE XML GeoData is what
    // we want?  For now, this makes sense
}

void NITFImageInfo::compute()
{
    computeImageInfo();
    computeSegmentInfo();
}


std::vector<nitf::BandInfo> NITFImageInfo::getBandInfo() const
{
    const GetDisplayLutFromData getLUT(*mData);
    return getBandInfoImpl<GetDisplayLutFromData>(mData->getPixelType(), getLUT);
}

std::string NITFImageInfo::generateFieldKey(const std::string& field,
        const std::string& prefix, int index)
{
    std::ostringstream s;
    s << prefix << field;
    if (index >= 0)
        s << "[" << str::toString(index) << "]";
    return s.str();
}
}
