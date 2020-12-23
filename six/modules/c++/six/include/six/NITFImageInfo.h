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
#pragma once

#include <string>

#include <six/Types.h>
#include <six/Data.h>
#include <six/Utilities.h>
#include <six/NITFSegmentInfo.h>
#include <nitf/ImageSegmentComputer.h>

namespace six
{

/*!
 *  \class NITFImageInfo
 *  \brief Internal book-keeping for NITF imagery
 *
 *  This class has the logic that calculates the dimensions of
 *  each image segment.  It also can tell you practical information
 *  about the image itself.
 *
 *  This class is used for both complex and derived products
 *
 *
 */
class NITFImageInfo
{
public:
    /*!
     * \param data Data associated with the NITF
     * \param maxRows Maximum number of rows allowed in an image segment.
     * Only applies when there is more than one image segment (we're ensuring
     * that the 5 characters reserved for the row offset in ILOC don't
     * overflow, so if there is only one image segment, there is no row offset
     * and thus no restriction on the number of rows).  Defaults to ILOC_MAX.
     * \param maxSize Maximum number of bytes allows in an image segment.
     * Defaults to IS_SIZE_MAX which is the ~10 GB NITF limit.
     * \param computeSegments Whether or not to compute segmentation at
     * construction time.  Defaults to false.
     * \param rowsPerBlock The number of rows in a NITF block.  Set this to 0
     * if the NITF is not blocked.  This needs to be taken into account as it'll
     * affect segmentation since NITFs always write entire NITF blocks even
     * when there are fewer rows than a multiple of the block size.  Defaults to
     * 0 (no blocking).
     * \param colsPerBlock The number of columns in a NITF block.  Set this to
     * 0 if the NITF is not blocked.  This needs to be taken into account as
     * it'll affect segmentation since NITFs always write entire NITF blocks
     * even when there are fewer columns than a multiple of the block size.
     * Defaults to 0 (no blocking).
     */
    NITFImageInfo(Data* data,
                  size_t maxRows = Constants::ILOC_MAX,
                  uint64_t maxSize = Constants::IS_SIZE_MAX,
                  bool computeSegments = false,
                  size_t rowsPerBlock = 0,
                  size_t colsPerBlock = 0);

    size_t getNumBitsPerPixel() const
    {
        return mData->getNumBytesPerPixel() / mData->getNumChannels() * 8;
    }

    static
    std::string getPixelValueType(PixelType pixelType)
    {
        switch (pixelType)
        {
        case PixelType::RE32F_IM32F:
            return "R";
        case PixelType::RE16I_IM16I:
            return "SI";
        default:
            return "INT";
        }
    }

    std::string getPixelValueType() const
    {
        return getPixelValueType(mData->getPixelType());
    }

    static
    std::string getRepresentation(PixelType pixelType)
    {
        switch (pixelType)
        {
        case PixelType::MONO8LU:
        case PixelType::MONO8I:
        case PixelType::MONO16I:
            return "MONO";
        case PixelType::RGB8LU:
            return "RGB/LUT";
        case PixelType::RGB24I:
            return "RGB";
        default:
            return "NODISPLY";
        }
    }

    std::string getRepresentation() const
    {
        return getRepresentation(mData->getPixelType());
    }

    static
    std::string getMode(PixelType pixelType)
    {
        switch (pixelType)
        {
        case PixelType::RGB8LU:
        case PixelType::MONO8LU:
        case PixelType::MONO8I:
        case PixelType::MONO16I:
            return "B";
        default:
            return "P";
        }
    }

    std::string getMode() const
    {
        return getMode(mData->getPixelType());
    }

    Data* getData() const
    {
        return mData;
    }

    std::vector<NITFSegmentInfo> getImageSegments() const
    {
        return mImageSegments;
    }

    /*!
     * Adds a segment.  Note that the segmentation layout computed at
     * construction time will no longer hold, so this should only be used
     * when 'computeSegments' was false at construction or the segmentation
     * layout is otherwise not being used.
     *
     * \param info The segment info to add
     */
    void addSegment(const NITFSegmentInfo& info)
    {
        mImageSegments.push_back(info);
    }

    size_t getStartIndex() const
    {
        return mStartIndex;
    }

    void setStartIndex(size_t index)
    {
        mStartIndex = index;
    }

    //! \return Number of total bytes in the image
    uint64_t getNumBytesTotal() const
    {
        return mSegmentComputer.getNumBytesTotal();
    }

    //! This is the total number of rows we can have in a NITF segment
    size_t getNumRowsLimit() const
    {
        return mSegmentComputer.getNumRowsLimit();
    }

    //! \return Max number of bytes that each image segment can be
    uint64_t getMaxNumBytesPerSegment() const
    {
        return mSegmentComputer.getMaxNumBytesPerSegment();
    }

    std::vector<nitf::BandInfo> getBandInfo() const;

    // We have to do this a little strangely with a functor rather than just
    // always sending in the display LUT because that method will throw for
    // ComplexData
    template <typename GetDisplayLutT>
    static
    std::vector<nitf::BandInfo>
    getBandInfoImpl(PixelType pixelType,
                    const GetDisplayLutT& getDisplayLUT);

    //!  File security classification system
    static const std::string CLSY;
    //!  File security codewords
    static const std::string CODE;
    //!  File control and handling
    static const std::string CTLH;
    //!  File releasing instructions
    static const std::string REL;
    //!  File security declassification type
    static const std::string DCTP;
    //!  File security declassification date
    static const std::string DCDT;
    //!  File security declassification exemption
    static const std::string DCXM;
    //!  File security downgrade
    static const std::string DG;
    //!  File security downgrade date
    static const std::string DGDT;
    //!  File security classification text
    static const std::string CLTX;
    //!  File security classification Authority type
    static const std::string CATP;
    //!  File security classification Authority
    static const std::string CAUT;
    //!  File security reason
    static const std::string CRSN;
    //!  File security source date
    static const std::string SRDT;
    //!  File security control number
    static const std::string CTLN;

    //! Utility that generates a key for the given field, with optional prefix and index
    static std::string generateFieldKey(const std::string& field,
            const std::string& prefix = "", int index = -1);

private:
    /*!
     *  This function figures out the parameters for each segment
     *  The algorithm follows the document.  The document does not
     *  state that they mean us to use WGS84 for LLA, but we assume
     *  that that is the intention
     *
     */
    void computeSegmentInfo();

    /*!
     *  This function transforms the LLA corners
     *  for the first row first column,
     *  and the first row last column, and turns those
     *  into ECEF, uses linear interpolation
     *
     */
    void computeSegmentCorners();

private:
    Data* const mData;

    const nitf::ImageSegmentComputer mSegmentComputer;

    size_t mStartIndex;

    /*!
     *  This is a vector of segment information that is used to get
     *  the per-segment info for populating/reading from a NITF
     *
     *  Note that the number of segments has a hard limit of 999
     */
    std::vector<NITFSegmentInfo> mImageSegments;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// WHAT FOLLOWS IS IMPLEMENTATION DETAIL
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

template <typename GetDisplayLutT>
std::vector<nitf::BandInfo>
NITFImageInfo::getBandInfoImpl(PixelType pixelType,
                               const GetDisplayLutT& getDisplayLUT)
{
    std::vector<nitf::BandInfo> bands;

    switch (pixelType)
    {
    case PixelType::RE32F_IM32F:
    case PixelType::RE16I_IM16I:
    {
        nitf::BandInfo band1;
        band1.getSubcategory().set("I");
        nitf::BandInfo band2;
        band2.getSubcategory().set("Q");

        bands.push_back(band1);
        bands.push_back(band2);
    }
        break;
    case PixelType::RGB24I:
    {
        nitf::BandInfo band1;
        band1.getRepresentation().set("R");

        nitf::BandInfo band2;
        band2.getRepresentation().set("G");

        nitf::BandInfo band3;
        band3.getRepresentation().set("B");

        bands.push_back(band1);
        bands.push_back(band2);
        bands.push_back(band3);
    }
        break;

    case PixelType::MONO8I:
    case PixelType::MONO16I:
    {
        nitf::BandInfo band1;
        band1.getRepresentation().set("M");
        bands.push_back(band1);
    }
        break;

    case PixelType::MONO8LU:
    {
        const LUT* lutPtr = getDisplayLUT();
        //If LUT is nullptr, we have a predefined LookupTable.
        //No LUT to write into NITF, so setting to MONO
        if (lutPtr == nullptr)
        {
            nitf::BandInfo band1;
            band1.getRepresentation().set("M");
            bands.push_back(band1);
        }
        else
        {
            // TODO: Why do we need to byte swap here?  If it is required, could
            //       we avoid the clone and byte swap and instead index into
            //       the LUT in the opposite order?
            std::unique_ptr<LUT> lut(lutPtr->clone());
            sys::byteSwap(reinterpret_cast<std::byte*>(lut->getTable()),
                          static_cast<unsigned short>(lut->elementSize),
                          lut->numEntries);

            if (lut->elementSize != sizeof(short))
            {
                throw except::Exception(Ctxt(
                    "Unexpected element size: " +
                    std::to_string(lut->elementSize)));
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
            //I would like to set it this way but it does not seem to work.
            //Using the init function instead.
            //band1.getRepresentation().set("LU");
            //band1.getLookupTable().setTable(table, 2, lut.numEntries);

            nitf::BandInfo band1;
            band1.init("LU", "", "", "",
                static_cast<uint32_t>(lut->elementSize),
                static_cast<uint32_t>(lut->numEntries),
                lookupTable);
            bands.push_back(band1);
        }
    }
    break;

    case PixelType::RGB8LU:
    {
        const LUT* const lut = getDisplayLUT();

        if (lut == nullptr)
        {
            //If LUT is nullptr, we have a predefined LookupTable.
            //No LUT to write into NITF, so setting to MONO
            nitf::BandInfo band1;
            band1.getRepresentation().set("M");
            bands.push_back(band1);
        }
        else
        {
            if (lut->elementSize != 3)
            {
                throw except::Exception(Ctxt(
                    "Unexpected element size: " +
                    std::to_string(lut->elementSize)));
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

            //I would like to set it this way but it does not seem to work.
            //Using the init function instead.
            //band1.getRepresentation().set("LU");
            //band1.getLookupTable().setTable(table, 3, lut->numEntries);
            
            nitf::BandInfo band1;
            band1.init("LU", "", "", "",
                static_cast<uint32_t>(lut->elementSize),
                static_cast<uint32_t>(lut->numEntries),
                lookupTable);
            bands.push_back(band1);
        }
    }
    break;

    default:
        throw except::Exception(Ctxt("Unknown pixel type"));
    }

    for (size_t i = 0; i < bands.size(); ++i)
    {
        bands[i].getImageFilterCondition().set("N");
    }
    return bands;
}
}


