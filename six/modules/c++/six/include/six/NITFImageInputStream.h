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
#ifndef __SIX_NITF_IMAGE_INPUT_STREAM_H__
#define __SIX_NITF_IMAGE_INPUT_STREAM_H__

#include <std/span>

#include <scene/sys_Conf.h>
#include <import/six.h>
#include <import/nitf.hpp>
#include <import/io.h>

#include <mem/ScopedArray.h>

namespace six
{

/*!
 *  \class NITFImageInputStream
 *  \brief Utility to copy NITF image data into a SICD/SIDD
 *
 *  Adapter from a NITF ImageReader to an InputStream.  This lets
 *  us use a NITF image segment as a source for WriteControl::save()
 *
 */
class NITFImageInputStream : public io::InputStream
{
public:

    /*!
     *  Takes in an ImageSubheader for the image of interest within
     *  the nitf Record, along with an ImageReader that was initialized
     *  to read the image data itself.
     *
     */
    NITFImageInputStream(nitf::ImageSubheader subheader,
            nitf::ImageReader imageReader);

    //!  Destructor
    virtual ~NITFImageInputStream() noexcept = default;

    NITFImageInputStream(const NITFImageInputStream&) = delete;
    NITFImageInputStream& operator=(const NITFImageInputStream&) = delete;

    //!  How many bytes in the image
    int64_t available();

    //!  Read N bytes from a NITF file
    ptrdiff_t read(std::byte* b, size_t len);
    ptrdiff_t read(sys::byte* b, size_t len);
    ptrdiff_t read(std::span<sys::byte>);

protected:

    ptrdiff_t readRow();

    nitf::ImageSubheader mSubheader;
    nitf::ImageReader mReader;
    int64_t mAvailable;
    size_t mRowBufferRemaining, mRowSize, mRowOffset;
    std::unique_ptr<sys::ubyte[]> mRowBuffer;
    nitf::SubWindow mWindow;
    std::unique_ptr<uint32_t[]> mBandList;
};

}
#endif

