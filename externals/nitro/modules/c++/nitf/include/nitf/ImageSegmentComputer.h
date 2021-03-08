/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_IMAGE_SEGMENT_COMPUTER_H__
#define __NITF_IMAGE_SEGMENT_COMPUTER_H__

#include <vector>
#include <string>

#include <nitf/System.hpp>

namespace nitf
{
/*!
 * \class ImageSegmentComputer
 *
 * \brief Contains logic to take a large image and determine the segmentation
 * required in order to stay within the NITF limits for total image segment
 * size (~10 GB) and max number of rows (because we will be creating multiple
 * image segments all attached to each other, we'll use ILOC to relate them
 * together, but ILOC only has five digits for row so multi-segment products
 * can't be more than 99,999 rows).
 *
 * Currently only supports segmenting in the row direction.
 */
struct ImageSegmentComputer /*final*/   // no "final", SWIG doesn't like it
{
    ImageSegmentComputer(const ImageSegmentComputer&) = delete;
    ImageSegmentComputer& operator=(const ImageSegmentComputer&) = delete;

    /*!
     * Max number of rows that can fit in an image segment and still be
     * representable in the following image segment via the ILOC field
     */
    static const size_t ILOC_MAX;

    /*!
     * Max number of bytes for an image segment's data (per the NITF spec -
     * this is due to the length field only being 10 digits)
     */
    static const uint64_t NUM_BYTES_MAX;

    /*!
     * \class Segment
     *
     * \brief Represents information about a segment in terms of its size and
     * position with respect to the global image
     */
    struct Segment
    {
        //! First row in the image segment in real space
        size_t firstRow;

        /*!
         * Row offset in the CCS (ILOC R)
         * When you are attached to another segment, ILOC is with respect to
         * that segment.  We will always attach to the previous segment,
         * so this will simply be the number of rows in that previous segment.
         */
        size_t rowOffset;

        //! Number of rows in this segment
        size_t numRows;

        //! \return The end row (exclusive) of the image segment
        size_t endRow() const noexcept
        {
            return firstRow + numRows;
        }

        //! \return The proper population of ILOC for this segment
        std::string getILOC() const;

        /*!
         * Given a global pixel range of
         * [rangeStartRow, rangeStartRow + rangeNumRows), determines if this
         * range overlaps with this segment and, if so, what rows overlap with
         * it
         *
         * \param rangeStartRow The inclusive global start row of the range of
         * interest
         * \param rangeNumRows The number of rows in the range of interest
         * \param[out] firstGlobalRowInThisSegment If the range does appear in
         * this segment, the first global row of the range that's in the
         * segment (if the range starts prior to this segment, it'll be the
         * first row of the segment.  If the range starts inside the segment,
         * it'll be the first row of the range).  If the range does not appear
         * in this segment, this is set to numeric_limits<size_t>::max().
         * \param[out] numRowsInThisSegment The number of rows of the range
         * which appear in this segment
         *
         * \return True if the range overlaps with this segment, false
         * otherwise
         */
        bool isInRange(size_t rangeStartRow,
                       size_t rangeNumRows,
                       size_t& firstGlobalRowInThisSegment,
                       size_t& numRowsInThisSegment) const noexcept;

        /*!
         * Convenience static implementation
         *
         * \param firstRow First row (inclusive) of the segment of interest
         * \param endRow End row (exclusive) of the segment of interest
         * \param rangeStartRow The inclusive global start row of the range of
         * interest
         * \param rangeNumRows The number of rows in the range of interest
         * \param[out] firstGlobalRowInThisSegment If the range does appear in
         * this segment, the first global row of the range that's in the
         * segment (if the range starts prior to this segment, it'll be the
         * first row of the segment.  If the range starts inside the segment,
         * it'll be the first row of the range).  If the range does not appear
         * in this segment, this is set to numeric_limits<size_t>::max().
         * \param[out] numRowsInThisSegment The number of rows of the range
         * which appear in this segment
         *
         * \return True if the range overlaps with this segment, false
         * otherwise
         */
        static
        bool isInRange(size_t firstRow,
                       size_t endRow,
                       size_t rangeStartRow,
                       size_t rangeNumRows,
                       size_t& firstGlobalRowInThisSegment,
                       size_t& numRowsInThisSegment) noexcept;
    };

    /*!
     * \param numRows Number of total rows in the image
     * \param numCols Number of columns in the image
     * \param numBytesPerPixel Number of bytes per pixel
     * \param maxRows Maximum number of rows allowed in an image segment.
     * Only applies when there is more than one image segment (we're ensuring
     * that the 5 characters reserved for the row offset in ILOC don't
     * overflow, so if there is only one image segment, there is no row offset
     * and thus no restriction on the number of rows).  Defaults to ILOC_MAX.
     * \param maxSize Maximum number of bytes allows in an image segment.
     * Defaults to NUM_BYTES_MAX which is the ~10 GB NITF limit.
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
    ImageSegmentComputer(size_t numRows,
                         size_t numCols,
                         size_t numBytesPerPixel,
                         size_t maxRows = ILOC_MAX,
                         uint64_t maxSize = NUM_BYTES_MAX,
                         size_t rowsPerBlock = 0,
                         size_t colsPerBlock = 0);

    //! \return Segment layout
    const std::vector<Segment>& getSegments() const noexcept
    {
        return mSegments;
    }

    //! \return Number of total bytes in the image
    uint64_t getNumBytesTotal() const noexcept
    {
        return mNumBytesTotal;
    }

    //! \return Total number of rows we can have in a NITF segment
    size_t getNumRowsLimit() const noexcept
    {
        return mNumRowsLimit;
    }

    //! \return Max number of bytes that each image segment can be
    uint64_t getMaxNumBytesPerSegment() const noexcept
    {
        return mMaxNumBytesPerSegment;
    }

private:
    // This is the actual size of a dimension (row or column)
    // taking into account the possible blocking (pixels)
    static
    size_t getActualDim(size_t dim, size_t numDimsPerBlock) noexcept;

    void computeImageInfo();

    void computeSegmentInfo();

private:
    //! Total number of rows in the image
    const size_t mNumRows;

    //! Total number of columns in the image
    const size_t mNumCols;

    //! Number of bytes per pixel
    const size_t mNumBytesPerPixel;

    //! Total number of rows we can have in a NITF segment
    size_t mNumRowsLimit;

    //! Number of columns once blocking is taken into account
    const size_t mNumColsPaddedForBlocking;

    //! Block size in the row direction, pixels
    const size_t mNumRowsPerBlock;

    //! Total size in bytes that each product seg can be
    const uint64_t mMaxNumBytesPerSegment;

    //! Number of bytes in the product
    const uint64_t mNumBytesTotal;

    //! Segment layout
    std::vector<Segment> mSegments;
};
}

#endif
