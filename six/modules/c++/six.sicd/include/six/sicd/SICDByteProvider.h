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

#ifndef __SIX_SICD_BYTE_PROVIDER_H__
#define __SIX_SICD_BYTE_PROVIDER_H__

#include <vector>

#include <six/NITFSegmentInfo.h>
#include <six/sicd/ComplexData.h>

namespace six
{
namespace sicd
{
/*!
 * \class SICDByteProvider
 * \brief Used to provide corresponding raw NITF bytes (including NITF headers)
 * when provided with some AOI of the pixel data.  The idea is that if
 * getBytes() is called multiple times, eventually for the entire image, the
 * raw bytes provided back will be the entire NITF file.  This abstraction is
 * useful if separate threads, processes, or even machines have only portions of
 * the SICD pixel data and are all trying to write out a single file; in that
 * scenario, this class provides all the raw bytes corresponding to the caller's
 * AOI, including NITF headers if necessary.  The caller does not need to
 * understand anything about the NITF file layout in order to write out the
 * file.  The bytes are intentionally provided back as a series of pointers
 * rather than one contiguous block of memory in order to not perform any
 * copies.
 */
class SICDByteProvider
{
public:
    /*!
     * \class NITFBuffer
     * \brief Represents a pointer to raw NITF bytes and its length
     */
    struct NITFBuffer
    {
        /*!
         * Initializes to an empty buffer
         */
        NITFBuffer();

        /*!
         * Initializes to the specified pointer and size.  No copy is made and
         * this object does not take ownership.
         *
         * \param data The raw bytes of data
         * \param numBytes The number of bytes of contiguous data this
         * represents
         */
        NITFBuffer(const void* data, size_t numBytes);

        const void* mData;
        size_t mNumBytes;
    };

    /*!
     * \class NITFBufferList
     * \brief Represents a sequence of buffers which appear in contiguous order
     * in the NITF (the underlying pointers are not contiguous)
     */
    struct NITFBufferList
    {
        //! The buffers
        std::vector<NITFBuffer> mBuffers;

        /*!
         * \return The total number of bytes across all the buffers
         */
        size_t getTotalNumBytes() const;

        /*!
         * \return Whether or not the buffer list is empty
         */
        bool empty() const
        {
            return mBuffers.empty();
        }

        /*!
         * Clear the buffers
         */
        void clear()
        {
            mBuffers.clear();
        }

        /*!
         * Push data onto the buffer list
         *
         * \param data The raw bytes
         * \param numBytes The number of bytes of data
         */
        void pushBack(const void* data, size_t numBytes)
        {
            mBuffers.push_back(NITFBuffer(data, numBytes));
        }

        /*!
         * Push data onto the buffer list
         *
         * \tparam DataT The type of data
         *
         * \param data The raw bytes
         */
        template <typename DataT>
        void pushBack(const std::vector<DataT>& data)
        {
            pushBack(data.empty() ? NULL : &data[0],
                     data.size() * sizeof(DataT));
        }

        /*!
         * Get the number of blocks of data of size 'blockSize'.  In cases
         * where the block size is not an even multiple of the total number of
         * bytes, the last block will be larger than the block size (rather
         * than there being one more block which is smaller than the block
         * size).  This is intentional in order to make this easily usable with
         * Amazon's S3 storage with multipart uploads where there is a minimum
         * part size (there may be multiple machines, all with their portion
         * of the SICD, performing a multipart upload of their parts, and
         * only the last overall part of the object can be less than the
         * minimum part size).
         *
         * \param blockSize The desired block size
         *
         * \return The associated number of blocks
         */
        size_t getNumBlocks(size_t blockSize) const;

        /*!
         * Get the number of bytes in the specified block.  All blocks will be
         * the same size except for the last block (see getNumBlocks() for
         * details).
         *
         * \param blockSize The desired block size
         * \param blockIdx The 0-based block index
         *
         * \return The number of bytes in this block
         */
        size_t getNumBytesInBlock(size_t blockSize, size_t blockIdx) const;

        /*!
         * Returns a pointer to contiguous memory associated with the desired
         * block.  If this block lies entirely within a NITFBuffer, no copy
         * is performed.  Otherwise, the scratch buffer is resized, the bytes
         * are copied into this, and a pointer to the scratch buffer is
         * returned.
         *
         * \param blockSize The desired block size.  See getNumBlocks() for a
         * description on the behavior of the last block.
         * \param blockIdx The 0-based block index
         * \param[out] scratch Scratch buffer.  This will be resized and used
         * if the underlying memory for this block is not contiguous (i.e. it
         * spans NITFBuffers).
         * \param[out] numBytes The number of bytes in this block
         *
         * \return A pointer to contiguous memory associated with this block
         */
        const void* getBlock(size_t blockSize,
                             size_t blockIdx,
                             std::vector<sys::byte>& scratch,
                             size_t& numBytes) const;
    };

    /*!
     * Constructor
     *
     * \param data Representation of the complex data
     * \param schemPaths Directories or files of schema locations
     * \param maxProductSize The max number of bytes in an image segment.
     * By default this is set automatically for you based on NITF file rules.
     */
    SICDByteProvider(const ComplexData& data,
                     const std::vector<std::string>& schemaPaths,
                     size_t maxProductSize = 0);

    /*!
     * Constructor
     * This option allows you to pass in an initialized writer,
     * in case you need something specific in the header
     *
     * \param writer Initialized NITFWriteControl
     * \param schemaPaths Directories or files of schema locations
     */
    SICDByteProvider(const NITFWriteControl& writer,
                     const std::vector<std::string>& schemaPaths);

    /*!
     * \return The total number of bytes in the NITF
     */
    nitf::Off getFileNumBytes() const
    {
        return mFileNumBytes;
    }

    /*!
     * Given a range of rows from [startRow, startRow + numRows), provide the
     * number of bytes that will appear in the NITF on disk (including NITF
     * file header, image subheader(s), and DES subheader and data).  Calling
     * this method repeatedly, eventually providing the entire range of the
     * image, will produce the total number of bytes in the full NITF.
     *
     * \param startRow The global start row in pixels as to where these pixels
     * are in the image.  If this is a multi-segment NITF, this is still simply
     * the global pixel location.
     * \param numRows The number of rows
     *
     * \return The associated number of bytes in the NITF
     */
    nitf::Off getNumBytes(size_t startRow, size_t numRows) const;

    /*!
     * The caller provides an AOI of the pixel data.  This method provides back
     * a list of contiguous buffers corresponding to the raw NITF bytes for
     * this portion of the file.  If this AOI is in the middle of an image
     * segment, this will be simply a buffer list of length 1 consisting of the
     * input pointer (no copy occurs).  Otherwise, pointers to various headers
     * (file header, image subheader(s), DES subheader and data) will
     * be in the buffer list before, in the middle, and/or after the image
     * data.  If this method is called multiple times with AOIs that
     * eventually consist of the entire image, and the raw bytes are written
     * out to disk in order with respect to the start pixel rows this method is
     * called with (or out of order but seeking to the provided file offset
     * each time), and in the order contained in the buffer list, it will form
     * a valid NITF.
     *
     * \note This method does not perform byte swapping on the pixel data for
     * efficiency reasons, but NITFs are written out in big endian order.  This
     * means that on a little endian system, you must byte swap the pixel data
     * prior to calling this method.
     *
     * \param imageData The image data pixels to write.  The underlying type
     * will be complex short or complex float based on the complex data sent
     * in during initialize.  Must be in big endian order.
     * \param startRow The global start row in pixels as to where these pixels
     * are in the image.  If this is a multi-segment NITF, this is still simply
     * the global pixel location.
     * \param numRows The number of rows in the provided 'imageData'
     * \param[out] fileOffset The offset in bytes in the NITF where these
     * buffers should be written
     * \param[out] buffers One or more pointers to raw bytes of data.  These
     * should be written out in the order they are provided in the buffer list.
     * The pointers point to the provided pixel data and, if required, one or
     * more NITF headers.  No copies occur, so these buffers are only valid for
     * the lifetime of this provider object (since this object owns the raw
     * bytes for the NITF headers) and the lifetime of the passed-in image
     * data.
     */
    void getBytes(const void* imageData,
                  size_t startRow,
                  size_t numRows,
                  nitf::Off& fileOffset,
                  NITFBufferList& buffers) const;

private:
    static size_t getNumBytesPerRow(const NITFWriteControl& writer);

private:
    const size_t mNumBytesPerRow;

    std::vector<sys::byte> mFileHeader;
    std::vector<std::vector<sys::byte> > mImageSubheaders;
    std::vector<sys::byte> mDesSubheaderAndData;
    std::vector<nitf::Off> mImageSubheaderFileOffsets;
    std::vector<NITFSegmentInfo> mImageSegmentInfo;
    nitf::Off mDesSubheaderFileOffset;
    nitf::Off mFileNumBytes;
};
}
}

#endif
