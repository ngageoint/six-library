/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#include "nitf/SegmentReader.hpp"

using namespace nitf;

SegmentReader::SegmentReader(const SegmentReader & x)
{
    setNative(x.getNative());
}

SegmentReader & SegmentReader::operator=(const SegmentReader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

SegmentReader::SegmentReader(nitf_SegmentReader * x)
{
    setNative(x);
    getNativeOrThrow();
}

SegmentReader::~SegmentReader(){}


void SegmentReader::read
(
    NITF_DATA *buffer,           /*!< Buffer to hold data */
    size_t count                /*!< Amount of data to return */
) throw (nitf::NITFException)
{
    if (!nitf_SegmentReader_read(getNativeOrThrow(), buffer, count, &error))
        throw nitf::NITFException(&error);
}


nitf::Off SegmentReader::seek
(
    nitf::Off offset,                 /*!< The seek offset */
    int whence                   /*!< Starting at (SEEK_SET, SEEK_CUR, SEEK_END)*/
) throw (nitf::NITFException)
{
    offset = nitf_SegmentReader_seek(getNativeOrThrow(), offset, whence, &error);
    if (offset < 0)
        throw nitf::NITFException(&error);
    return offset;
}


nitf::Off SegmentReader::tell()
{
    return nitf_SegmentReader_tell(getNativeOrThrow(), &error);
}


nitf::Off SegmentReader::getSize()
{
    return nitf_SegmentReader_getSize(getNativeOrThrow(), &error);
}
