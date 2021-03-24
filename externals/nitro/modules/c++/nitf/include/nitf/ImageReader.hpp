/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __NITF_IMAGE_READER_HPP__
#define __NITF_IMAGE_READER_HPP__
#pragma once

#include <assert.h>

#include <vector>
#include <string>
#include <memory>

#include <std/span>

#include "nitf/coda-oss.hpp"
#include "nitf/ImageReader.h"
#include "nitf/Object.hpp"
#include "nitf/BlockingInfo.hpp"
#include "nitf/System.hpp"

/*!
 *  \file ImageReader.hpp
 *  \brief  Contains wrapper implementation for ImageReader
 */

namespace nitf
{
    template<typename T>
    class BufferList /*final*/   // no "final", SWIG doesn't like it
    {
        std::vector<T*> buffer;
        std::vector<std::unique_ptr<T[]>> buffer_;

    public:
        BufferList(size_t nBands)
            : buffer(nBands), buffer_(nBands)
        {
        }

        void initialize(size_t subWindowSize)
        {
            for (size_t i = 0; i < size(); i++)
            {
                buffer_[i].reset(new T[subWindowSize]);
                buffer[i] = buffer_[i].get();
            }
        }

        size_t size() const noexcept
        {
            assert(buffer.size() == buffer_.size());
            return buffer.size();
        }

        T** data() noexcept
        {
            return buffer.data();
        }

        T*& operator[](size_t i)noexcept
        {
            return buffer[i];
        }
    };

/*!
 *  \class ImageReader
 *  \brief  The C++ wrapper for the nitf_ImageReader
 */
DECLARE_CLASS(ImageReader)
{
public:
    //! Copy constructor
    ImageReader(const ImageReader & x);

    //! Assignment Operator
    ImageReader & operator=(const ImageReader & x);

    //! Set native object
    ImageReader(nitf_ImageReader * x);

    ~ImageReader() = default;

    //! Get the blocking info
    nitf::BlockingInfo getBlockingInfo() const;

    /*!
     *  Read a sub-window.  See ImageIO::read for more details.
     *  \param  subWindow  The sub-window to read
     *  \param  user  User-defined data buffers for read
     *  \param  padded  Returns TRUE if pad pixels may have been read
     */
    void read(const nitf::SubWindow & subWindow, uint8_t ** user, int * padded);
    void read(const nitf::SubWindow & subWindow, sys::byte** user, int * padded)
    {
        void* user_ = user;
        read(subWindow, static_cast<uint8_t**>(user_), padded);
    }
    void read(const nitf::SubWindow& subWindow, std::byte** user, int* padded)
    {
        void* user_ = user;
        read(subWindow, static_cast<uint8_t**>(user_), padded);
    }

    /*!
     *  Read a block directly from file
     *  \param blockNumber
     *  \param blockSize  Returns block size
     *  \return The read block 
     *          (something must be done with buffer before next call)
     */
    const uint8_t* readBlock(uint32_t blockNumber, 
                                 uint64_t* blockSize);

    //!  Set read caching
    void setReadCaching();

private:
    mutable nitf_Error error{};
    ImageReader() = default;
};

}
#endif
