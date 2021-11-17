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
#pragma once

#include <string>

#include <nitf/ImageSubheader.hpp>

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
    NITFImageInfo(const NITFImageInfo&) = delete;
    NITFImageInfo& operator=(const NITFImageInfo&) = delete;

    size_t getNumBitsPerPixel() const
    {
        return mData->getNumBytesPerPixel() / mData->getNumChannels() * 8;
    }

    static std::string getPixelValueType(PixelType pixelType);
    std::string getPixelValueType() const
    {
        return getPixelValueType(mData->getPixelType());
    }
    static nitf::PixelValueType getPixelType(PixelType pixelType);
    nitf::PixelValueType getPixelType() const
    {
        return getPixelType(mData->getPixelType());
    }

    static std::string getRepresentation(PixelType pixelType);
    std::string getRepresentation() const
    {
        return getRepresentation(mData->getPixelType());
    }
    static nitf::ImageRepresentation getImageRepresentation(PixelType pixelType);
    nitf::ImageRepresentation getImageRepresentation() const
    {
        return getImageRepresentation(mData->getPixelType());
    }

    static std::string getMode(PixelType pixelType);
    std::string getMode() const
    {
        return getMode(mData->getPixelType());
    }
    static nitf::BlockingMode getBlockingMode(PixelType pixelType);
    nitf::BlockingMode getBlockingMode() const
    {
        return getBlockingMode(mData->getPixelType());
    }

    const Data* getData() const
    {
        return mData;
    }
    Data* getData_()
    {
        return mData_;
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
    inline static const LUT* getDisplayLUT_(PixelType pixelType, const GetDisplayLutT& getDisplayLUT)
    {
        if ((pixelType == PixelType::MONO8LU) || (pixelType == PixelType::RGB8LU) || (pixelType == PixelType::AMP8I_PHS8I))
        {
            return getDisplayLUT();
        }
        return nullptr;
    }
    template <typename GetDisplayLutT>
    static std::vector<nitf::BandInfo> getBandInfoImpl(PixelType pixelType, const GetDisplayLutT& getDisplayLUT)
    {
        const LUT* lutPtr = getDisplayLUT_(pixelType, getDisplayLUT);
        return getBandInfoImpl_(pixelType, lutPtr);
    }

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
    const Data* const mData;
    Data* const mData_;

    const nitf::ImageSegmentComputer mSegmentComputer;

    size_t mStartIndex;

    /*!
     *  This is a vector of segment information that is used to get
     *  the per-segment info for populating/reading from a NITF
     *
     *  Note that the number of segments has a hard limit of 999
     */
    std::vector<NITFSegmentInfo> mImageSegments;

    static std::vector<nitf::BandInfo> getBandInfoImpl_(PixelType, const LUT* = nullptr);
};

}

#endif
