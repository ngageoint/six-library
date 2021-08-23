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
#include <six/NITFImageInfo.h>

#include <memory>
#include <sstream>
#include <limits>
#include <string>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <str/Convert.h>
#include <scene/Utilities.h>

namespace
{
struct GetDisplayLutFromData final
{
    GetDisplayLutFromData(six::Data& data) :
        mData(data)
    {
    }

    GetDisplayLutFromData(const GetDisplayLutFromData&) = delete;
    GetDisplayLutFromData& operator=(const GetDisplayLutFromData&) = delete;

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
const std::string NITFImageInfo::CLSY = "CLSY";
//!  File security codewords
const std::string NITFImageInfo::CODE = "CODE";
//!  File control and handling
const std::string NITFImageInfo::CTLH = "CTLH";
//!  File releasing instructions
const std::string NITFImageInfo::REL = "REL";
//!  File security declassification type
const std::string NITFImageInfo::DCTP = "DCTP";
//!  File security declassification date
const std::string NITFImageInfo::DCDT = "DCDT";
//!  File security declassification exemption
const std::string NITFImageInfo::DCXM = "DCXM";
//!  File security downgrade
const std::string NITFImageInfo::DG = "DG";
//!  File security downgrade date
const std::string NITFImageInfo::DGDT = "DGDT";
//!  File security classification text
const std::string NITFImageInfo::CLTX = "CLTX";
//!  File security classification Authority type
const std::string NITFImageInfo::CATP = "CATP";
//!  File security classification Authority
const std::string NITFImageInfo::CAUT = "CAUT";
//!  File security reason
const std::string NITFImageInfo::CRSN = "CRSN";
//!  File security source date
const std::string NITFImageInfo::SRDT = "SRDT";
//!  File security control number
const std::string NITFImageInfo::CTLN = "CTLN";


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
    const Vector3 icp1 = scene::Utilities::latLonToECEF(corners.upperLeft);
    // (0, N)
    const Vector3 icp2 = scene::Utilities::latLonToECEF(corners.upperRight);
    // (M, N)
    const Vector3 icp3 = scene::Utilities::latLonToECEF(corners.lowerRight);
    // (M, 0)
    const Vector3 icp4 = scene::Utilities::latLonToECEF(corners.lowerLeft);

    const auto total = static_cast<double>(mData->getNumRows()) - 1.0;

    for (auto& imageSegment : mImageSegments)
    {
        const auto firstRow = static_cast<double>(imageSegment.getFirstRow());
        const auto wgt1 = (total - firstRow) / total;
        const auto wgt2 = firstRow / total;

        // This requires an operator overload for scalar * vector
        Vector3 ecef = wgt1 * icp1 + wgt2 * icp4;

        imageSegment.corners.upperLeft =
            scene::Utilities::ecefToLatLon(ecef);

        // Now do it for the first
        ecef = wgt1 * icp2 + wgt2 * icp3;

        imageSegment.corners.upperRight =
            scene::Utilities::ecefToLatLon(ecef);
    }

    size_t i = 0;
    const auto numIS = mImageSegments.size();
    for (i = 0; i < numIS - 1; i++)
    {
        LatLonCorners& theseCorners(mImageSegments[i].corners);
        const LatLonCorners& nextCorners(mImageSegments[i + 1].corners);

        theseCorners.lowerRight.setLatLon(nextCorners.upperRight);
        theseCorners.lowerLeft.setLatLon(nextCorners.upperLeft);
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
        s << "[" << std::to_string(index) << "]";
    return s.str();
}
}

static std::vector<nitf::BandInfo> getBandInfoImpl_REnF_IMnF()
{
    std::vector<nitf::BandInfo> bands;

    nitf::BandInfo band1;
    band1.getSubcategory().set("I");
    nitf::BandInfo band2;
    band2.getSubcategory().set("Q");

    bands.push_back(band1);
    bands.push_back(band2);

    return bands;
}

static std::vector<nitf::BandInfo> getBandInfoImpl_RGB24I()
{
    std::vector<nitf::BandInfo> bands;

    nitf::BandInfo band1;
    band1.getRepresentation().set("R");

    nitf::BandInfo band2;
    band2.getRepresentation().set("G");

    nitf::BandInfo band3;
    band3.getRepresentation().set("B");

    bands.push_back(band1);
    bands.push_back(band2);
    bands.push_back(band3);

    return bands;
}

static std::vector<nitf::BandInfo> getBandInfoImpl_MONOnI()
{
    std::vector<nitf::BandInfo> bands;

    nitf::BandInfo band1;
    band1.getRepresentation().set("M");
    bands.push_back(band1);

    return bands;
}

static std::vector<nitf::BandInfo> getBandInfoFromLUT(const six::LUT& lut, nitf::LookupTable& lookupTable)
{
    //I would like to set it this way but it does not seem to work.
    //Using the init function instead.
    //band1.getRepresentation().set("LU");
    //band1.getLookupTable().setTable(table, 2, lut.numEntries);
    nitf::BandInfo band1;
    band1.init("LU", "", "", "", static_cast<uint32_t>(lut.elementSize), static_cast<uint32_t>(lut.numEntries), lookupTable);
    std::vector<nitf::BandInfo> bands;
    bands.push_back(band1);

    return bands;
}

static std::vector<nitf::BandInfo> getBandInfoImpl_MONO8LU(const six::LUT* lutPtr)
{
    //If LUT is nullptr, we have a predefined LookupTable.
    //No LUT to write into NITF, so setting to MONO
    if (lutPtr == nullptr)
    {
        return getBandInfoImpl_MONOnI();
    }

    // TODO: Why do we need to byte swap here?  If it is required, could
    //       we avoid the clone and byte swap and instead index into
    //       the LUT in the opposite order?
    std::unique_ptr<six::LUT> lut(lutPtr->clone());
    void* pTable = lut->getTable();
    sys::byteSwap(static_cast<std::byte*>(pTable), static_cast<unsigned short>(lut->elementSize), lut->numEntries);

    if (lut->elementSize != sizeof(short))
    {
        throw except::Exception(Ctxt("Unexpected element size: " + std::to_string(lut->elementSize)));
    }

    nitf::LookupTable lookupTable(lut->elementSize, lut->numEntries);
    unsigned char* const table(lookupTable.getTable());
    for (size_t i = 0; i < lut->numEntries; ++i)
    {
        // Need two LUTS in the nitf, with high order
        // bits in the first and low order in the second
        const unsigned char* const entry = (*lut)[i];
        table[i] = entry[0];
        table[lut->numEntries + i] = entry[1];

    }

    return getBandInfoFromLUT(*lut, lookupTable);
}

static std::vector<nitf::BandInfo> getBandInfoImpl_RGB8LU(const six::LUT* lut)
{
    if (lut == nullptr)
    {
        //If LUT is nullptr, we have a predefined LookupTable.
        //No LUT to write into NITF, so setting to MONO
        return getBandInfoImpl_MONOnI();
    }

    if (lut->elementSize != 3)
    {
        throw except::Exception(Ctxt("Unexpected element size: " + std::to_string(lut->elementSize)));
    }

    nitf::LookupTable lookupTable(lut->elementSize, lut->numEntries);
    unsigned char* const table(lookupTable.getTable());
    for (size_t i = 0, k = 0; i < lut->numEntries; ++i)
    {
        for (size_t j = 0; j < lut->elementSize; ++j, ++k)
        {
            // Need to transpose the lookup table entries
            table[j * lut->numEntries + i] = lut->getTable()[k];
        }
    }

    return getBandInfoFromLUT(*lut, lookupTable);
}

//static std::vector<nitf::BandInfo> getBandInfoImpl_AMP8I_PHS8I(const six::LUT* lutPtr)
//{
//    std::vector<nitf::BandInfo> bands;
//    if (lutPtr == nullptr)
//    {
//        //If LUT is nullptr, we have a predefined LookupTable.
//        //No LUT to write into NITF, so setting to MONO
//        return getBandInfoImpl_MONOnI();
//    }
//
//    if (lutPtr->elementSize != sizeof(short))
//    {
//        throw except::Exception(Ctxt("Unexpected element size: " + std::to_string(lutPtr->elementSize)));
//    }
//
//    // TODO
//    return bands;
//}

std::vector<nitf::BandInfo> six::NITFImageInfo::getBandInfoImpl_(PixelType pixelType, const LUT* pLUT)
{
    std::vector<nitf::BandInfo> bands;

    switch (pixelType)
    {
    case PixelType::RE32F_IM32F:
    case PixelType::RE16I_IM16I:
    {
        bands = getBandInfoImpl_REnF_IMnF();
    }
    break;
    case PixelType::RGB24I:
    {
        bands = getBandInfoImpl_RGB24I();
    }
    break;

    case PixelType::MONO8I:
    case PixelType::MONO16I:
    {
        bands = getBandInfoImpl_MONOnI();
    }
    break;

    case PixelType::MONO8LU:
    {
        bands = getBandInfoImpl_MONO8LU(pLUT);
    }
    break;

    case PixelType::RGB8LU:
    {
        bands = getBandInfoImpl_RGB8LU(pLUT);
    }
    break;

    //case PixelType::AMP8I_PHS8I:
    //{
    //    bands = getBandInfoImpl_AMP8I_PHS8I(pLUT);
    //}
    //break;

    default:
        throw except::Exception(Ctxt("Unknown pixel type"));
    }

    for (size_t i = 0; i < bands.size(); ++i)
    {
        bands[i].getImageFilterCondition().set("N");
    }
    return bands;
}
