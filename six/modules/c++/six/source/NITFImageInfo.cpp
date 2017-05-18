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


void NITFImageInfo::NITFImageInfo(Data* d,
                                  size_t maxRows,
                                  sys::Uint64_T,
                                  bool computeSegments,
                                  sys::Uint32_T rowsPerBlock,
                                  sys::Uint32_T colsPerBlock) :
        data(d), startIndex(0), numRowsLimit(maxRows), maxProductSize(maxSize),
        numRowsPerBlock(rowsPerBlock), numColsPerBlock(colsPerBlock)
{
    sys::Uint64_T rowSize = getActualDim(data->getNumRows(), rowsPerBlock);
    sys::Uint64_T colSize = getActualDim(data->getNumCols(), colsPerBlock);
    productSize = (sys::Uint64_T) data->getNumBytesPerPixel() * rowSize * colSize;
    if (computeSegments)
        compute();
}

size_t NITFImageInfo::getActualDim(size_t dim, size_t numDimsPerBlock)
{
    if (numDimsPerBlock == 0)
        return dim;
    else {
        // If we're blocking then we'll always write full blocks
        const size_t numBlocks = (dim/numDimsPerBlock) + (dim%numDimsPerBlock != 0);
        return numBlocks * numDimsPerBlock;
    }
}

void NITFImageInfo::computeImageInfo()
{
    // Consider possible blocking when determining the maximum number of rows
    size_t bytesPerRow =
        data->getNumBytesPerPixel() * getActualDim(data->getNumCols(), colsPerBlock);

    // This, to be safe, should be a 64bit number
    sys::Uint64_T maxRows = (sys::Uint64_T) maxProductSize / (sys::Uint64_T) bytesPerRow;

    if (maxRows == 0)
    {
        std::ostringstream ostr;
        ostr << "maxProductSize [" << maxProductSize << "] < bytesPerRow ["
             << bytesPerRow << "]";

        throw except::Exception(Ctxt(ostr.str()));
    }

    // Truncate back to a full block for the maxRows that will actually fit
    if (numRowsPerBlock) {
        const size_t numBlocksVert = maxRows / numRowsPerBlock;
        maxRows = (sys::Uint64_T)numBlocksVert * (sys::Uint64_T)numRowsPerBlock;
    }

    if (maxRows < numRowsLimit)
    {
        numRowsLimit = static_cast<size_t>(maxRows);
    }
}

void NITFImageInfo::computeSegmentInfo()
{
    // TODO: other conditions which should trigger segmentation:
    //   NCOLS >= 10E8   column direction segmentation, no numColsLimit logic in place
    //   NROWS >= 10E8   most likely would exceed maxProductSize (ncols < 100 unlikely)
    //                   and segment to numRowsLimit anyway
    if (productSize <= maxProductSize)
    {
        imageSegments.resize(1);
        imageSegments[0].numRows = data->getNumRows();
        imageSegments[0].firstRow = 0;
        imageSegments[0].rowOffset = 0;
        imageSegments[0].corners = data->getImageCorners();
    }

    else
    {
        // NOTE: See header for why rowOffset is always set to numRowsLimit
        //       for image segments 1 and above
        size_t numIS = (size_t) std::ceil(data->getNumRows()
                / (double) numRowsLimit);
        imageSegments.resize(numIS);
        imageSegments[0].numRows = numRowsLimit;
        imageSegments[0].firstRow = 0;
        imageSegments[0].rowOffset = 0;
        size_t i;
        for (i = 1; i < numIS - 1; i++)
        {
            imageSegments[i].numRows = numRowsLimit;
            imageSegments[i].firstRow = i * numRowsLimit;
            imageSegments[i].rowOffset = numRowsLimit;
        }

        imageSegments[i].firstRow = i * numRowsLimit;
        imageSegments[i].rowOffset = numRowsLimit;
        imageSegments[i].numRows = data->getNumRows() - (numIS - 1)
                * numRowsLimit;

    }

    computeSegmentCorners();
}

void NITFImageInfo::computeSegmentCorners()
{
    const LatLonCorners corners = data->getImageCorners();

    // (0, 0)
    Vector3 icp1 = scene::Utilities::latLonToECEF(corners.upperLeft);
    // (0, N)
    Vector3 icp2 = scene::Utilities::latLonToECEF(corners.upperRight);
    // (M, N)
    Vector3 icp3 = scene::Utilities::latLonToECEF(corners.lowerRight);
    // (M, 0)
    Vector3 icp4 = scene::Utilities::latLonToECEF(corners.lowerLeft);

    size_t numIS = imageSegments.size();
    double total = data->getNumRows() - 1.0;

    Vector3 ecef;
    size_t i;
    for (i = 0; i < numIS; i++)
    {
        size_t firstRow = imageSegments[i].firstRow;
        double wgt1 = (total - firstRow) / total;
        double wgt2 = firstRow / total;

        // This requires an operator overload for scalar * vector
        ecef = wgt1 * icp1 + wgt2 * icp4;

        imageSegments[i].corners.upperLeft =
            scene::Utilities::ecefToLatLon(ecef);

        // Now do it for the first
        ecef = wgt1 * icp2 + wgt2 * icp3;

        imageSegments[i].corners.upperRight =
            scene::Utilities::ecefToLatLon(ecef);
    }

    for (i = 0; i < numIS - 1; i++)
    {
        LatLonCorners& theseCorners(imageSegments[i].corners);
        const LatLonCorners& nextCorners(imageSegments[i + 1].corners);

        theseCorners.lowerRight.setLat(nextCorners.upperRight.getLat());
        theseCorners.lowerRight.setLon(nextCorners.upperRight.getLon());

        theseCorners.lowerLeft.setLat(nextCorners.upperLeft.getLat());
        theseCorners.lowerLeft.setLon(nextCorners.upperLeft.getLon());
    }

    // This last one is cake
    imageSegments[i].corners.lowerRight = corners.lowerRight;
    imageSegments[i].corners.lowerLeft = corners.lowerLeft;

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
    const GetDisplayLutFromData getLUT(*data);
    return getBandInfoImpl<GetDisplayLutFromData>(data->getPixelType(), getLUT);
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
