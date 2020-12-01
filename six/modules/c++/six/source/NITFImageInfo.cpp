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
        return mData.getDisplayLUT().get();
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
                             uint64_t maxSize,
                             bool computeSegments,
                             size_t rowsPerBlock,
                             size_t colsPerBlock) :
    mData(data),
    mSegmentComputer(data->getNumRows(),
                     data->getNumCols(),
                     data->getNumBytesPerPixel(),
                     maxRows,
                     maxSize,
                     rowsPerBlock,
                     colsPerBlock),
    mStartIndex(0)
{
    if (computeSegments)
    {
        computeSegmentInfo();
    }
}

void NITFImageInfo::computeSegmentInfo()
{
    // The segment computer figured out all the row sizes and offsets for us
    // We just need to compute the segment corners
    const std::vector<nitf::ImageSegmentComputer::Segment>& segments =
            mSegmentComputer.getSegments();

    mImageSegments.resize(segments.size());
    for (size_t ii = 0; ii < mImageSegments.size(); ++ii)
    {
        const nitf::ImageSegmentComputer::Segment& segment(segments[ii]);
        NITFSegmentInfo& imageSegment(mImageSegments[ii]);

        imageSegment.firstRow = segment.firstRow;
        imageSegment.rowOffset = segment.rowOffset;
        imageSegment.numRows = segment.numRows;
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
