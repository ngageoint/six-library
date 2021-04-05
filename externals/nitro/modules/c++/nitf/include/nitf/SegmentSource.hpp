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

#ifndef __NITF_SEGMENTSOURCE_HPP__
#define __NITF_SEGMENTSOURCE_HPP__
#pragma once

#include <string>
#include <vector>

#include "nitf/coda-oss.hpp"
#include "nitf/DataSource.hpp"
#include "nitf/SegmentSource.h"
#include "nitf/SegmentReader.hpp"
#include "nitf/IOHandle.hpp"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"

/*!
 *  \file SegmentSource.hpp
 *  \brief  Contains wrapper implementations for SegmentSources
 */
namespace nitf
{

//! SegmentSource === DataSource
typedef DataSource SegmentSource;

/*!
 *  \class SegmentMemorySource
 *  \brief  The C++ wrapper for the nitf_SegmentMemorySource.
 *
 *  The memory source class allows us to read directly from
 *  a data buffer.  In the event that this is a memory-mapped file,
 *  we will likely get a performance gain over the direct fd approach.
 *
 *  The constructor takes in a buffer, a size, and optionally a
 *  sampling factor (Typically, the factor will be applied most
 *  times during the case of memory mapping, although it may be used
 *  to sample down or cut the data into pieces).
 */
class SegmentMemorySource : public SegmentSource
{
public:
    /*!
     *  Constructor
     *  \param data     The memory buffer
     *  \param size     The size of the buffer
     *  \param start    The start offset
     *  \param byteSkip The amount of bytes to skip
     *  \param copyData Whether or not to make a copy of the data.  If this is
     *  false, the data must outlive the memory source.
     */
    SegmentMemorySource(const sys::byte* data, nitf::Off size, nitf::Off start,
        int byteSkip, bool copyData);
    template<typename TContainer>
    SegmentMemorySource(const TContainer& data, nitf::Off start,
                        int byteSkip, bool copyData);
};

/*!
 *  \class SegmentFileSource
 *  \brief  The C++ wrapper for the nitf_SegmentFileSource
 *
 *  The SegmentFileSource class is a SegmentSource that comes from an open
 *  file descriptor or handle.  Due to any number of constraints,
 *  we allow the creator to specify a start point, and a byte skip.
 */
class SegmentFileSource : public SegmentSource
{
public:
    /*!
     *  Constructor
     *  \param handle   The handle to store
     *  \param start    The location to seek to (as the beginning)
     *  \param byteSkip The number of bytes to skip
     */
    SegmentFileSource(nitf::IOHandle & io, nitf::Off start, int byteSkip);

    ~SegmentFileSource()
    {
    }
};

class SegmentReaderSource : public SegmentSource
{
public:
    /*!
     *  Constructor
     *  \param handle   The handle to store
     *  \param start    The location to seek to (as the beginning)
     *  \param byteSkip The number of bytes to skip
     */
    SegmentReaderSource(nitf::SegmentReader reader);

    ~SegmentReaderSource()
    {
    }
};
}
#endif
