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
#ifndef __SIX_NITF_IMAGE_INFO_H__
#define __SIX_NITF_IMAGE_INFO_H__

#include <six/Data.h>
#include <six/Utilities.h>
#include <six/NITFSegmentInfo.h>

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

    NITFImageInfo(Data* d,
                  size_t maxRows = Constants::ILOC_MAX,
                  sys::Uint64_T maxSize = Constants::IS_SIZE_MAX,
                  bool computeSegments = false) :
        data(d), startIndex(0), numRowsLimit(maxRows), maxProductSize(maxSize)
    {
        productSize = (sys::Uint64_T) data->getNumBytesPerPixel()
                * (sys::Uint64_T) data->getNumRows()
                * (sys::Uint64_T) data->getNumCols();
        if (computeSegments)
            compute();
    }

    size_t getNumBitsPerPixel() const
    {
        return data->getNumBytesPerPixel() / data->getNumChannels() * 8;
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
        return getPixelValueType(data->getPixelType());
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
        return getRepresentation(data->getPixelType());
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
        return getMode(data->getPixelType());
    }

    Data* getData() const
    {
        return data;
    }

    std::vector<NITFSegmentInfo> getImageSegments() const
    {
        return imageSegments;
    }

    void addSegment(NITFSegmentInfo info)
    {
        imageSegments.push_back(info);
    }

    size_t getStartIndex() const
    {
        return startIndex;
    }
    void setStartIndex(size_t index)
    {
        startIndex = index;
    }

    //! Number of bytes in the product
    sys::Uint64_T getProductSize() const
    {
        return productSize;
    }

    //! This is the total number of rows we can have in a NITF segment
    size_t getNumRowsLimit() const
    {
        return numRowsLimit;
    }

    //! This is the total size that each product seg can be
    sys::Uint64_T getMaxProductSize() const
    {
        return maxProductSize;
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
    static const char CLSY[];
    //!  File security codewords
    static const char CODE[];
    //!  File control and handling
    static const char CTLH[];
    //!  File releasing instructions
    static const char REL[];
    //!  File security declassification type
    static const char DCTP[];
    //!  File security declassification date
    static const char DCDT[];
    //!  File security declassification exemption
    static const char DCXM[];
    //!  File security downgrade
    static const char DG[];
    //!  File security downgrade date
    static const char DGDT[];
    //!  File security classification text
    static const char CLTX[];
    //!  File security classification Authority type
    static const char CATP[];
    //!  File security classification Authority
    static const char CAUT[];
    //!  File security reason
    static const char CRSN[];
    //!  File security source date
    static const char SRDT[];
    //!  File security control number
    static const char CTLN[];

    //! Utility that generates a key for the given field, with optional prefix and index
    static std::string generateFieldKey(const std::string& field,
            const std::string& prefix = "", int index = -1);

private:
    void computeImageInfo();

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

    /*!
     *  This function is called by the container to determine
     *  what the properties of the image segments will be.
     *
     */
    void compute();

    /*     NITFImageInfo() : data(NULL), startIndex(0), */
    /*         numRowsLimit(Constants::ILOC_MAX), */
    /*             maxProductSize(Constants::IS_SIZE_MAX)  {} */

    /*!
     *  By default, we use the IS_SIZE_MAX to determine the max product
     *  size for an image segment, and if we have to segment, we
     *  use the ILOC_MAX to determine the segment size (if that is
     *  smaller than the product size).  These calls give the
     *  user access to these limits and allows them to be overridden.
     *
     *  This method would typically only be used during product
     *  prototyping and testing.  It should not be used to artificially
     *  constrain actual products.
     *
     *  Do not attempt to use this method unless you understand the
     *  segmentation rules.
     *
     */
    /*      void setLimits(sys::Uint64_T maxSize, */
    /*                     size_t maxRows) */
    /*      { */

    /*          if (maxSize > Constants::IS_SIZE_MAX) */
    /*              throw except::Exception(Ctxt("You cannot exceed the IS_SIZE_MAX")); */

    /*          if (maxRows > Constants::ILOC_MAX) */
    /*              throw except::Exception(Ctxt("You cannot exceed the ILOC_MAX")); */

    /*          maxProductSize = maxSize; */
    /*          numRowsLimit = maxRows; */
    /*      } */

private:
    Data* data;

    size_t startIndex;

    //! Number of bytes in the product
    sys::Uint64_T productSize;

    //! This is the total number of rows we can have in a NITF segment
    size_t numRowsLimit;

    //! This is the total size that each product seg can be
    sys::Uint64_T maxProductSize;

    /*!
     *  This is a vector of segment information that is used to get
     *  the per-segment info for populating/reading from a NITF
     *
     *  Note that the number of segments has a hard limit of 999
     */
    std::vector<NITFSegmentInfo> imageSegments;
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
        nitf::BandInfo band1;
        const LUT* lutPtr = getDisplayLUT();
        //If LUT is NULL, we have a predefined LookupTable.
        //No LUT to write into NITF, so setting to MONO
        if (lutPtr == NULL)
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
            std::auto_ptr<LUT> lut(lutPtr->clone());
            sys::byteSwap(reinterpret_cast<sys::byte*>(lut->getTable()),
                          static_cast<unsigned short>(lut->elementSize),
                          lut->numEntries);

            if (lut->elementSize != sizeof(short))
            {
                throw except::Exception(Ctxt(
                    "Unexpected element size: " +
                    str::toString(lut->elementSize)));
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

            band1.init("LU", "", "", "",
                static_cast<nitf::Uint32>(lut->elementSize),
                static_cast<nitf::Uint32>(lut->numEntries),
                lookupTable);
            bands.push_back(band1);
        }
    }
    break;

    case PixelType::RGB8LU:
    {
        nitf::BandInfo band1;

        const LUT* const lut = getDisplayLUT();

        if (lut == NULL)
        {
            //If LUT is NULL, we have a predefined LookupTable.
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
                    str::toString(lut->elementSize)));
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

            band1.init("LU", "", "", "",
                static_cast<nitf::Uint32>(lut->elementSize),
                static_cast<nitf::Uint32>(lut->numEntries),
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

#endif

