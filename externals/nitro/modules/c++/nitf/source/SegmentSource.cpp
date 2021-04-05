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

#include "nitf/SegmentSource.hpp"

#include <gsl/gsl.h>

#include "nitf/System.hpp"

nitf::SegmentMemorySource::SegmentMemorySource(const sys::byte* data, nitf::Off size,
        nitf::Off start, int byteSkip, bool copyData)
{
    setNative(nitf_SegmentMemorySource_construct(data, size, start, byteSkip,
    		                                     copyData, &error));
    setManaged(false);
}

namespace nitf
{
template<>
SegmentMemorySource::SegmentMemorySource(const std::string& data,
    nitf::Off start, int byteSkip, bool copyData)
    : SegmentMemorySource(data.c_str(), gsl::narrow<nitf::Off>(data.size()), start, byteSkip, copyData)
{
}
template<>
SegmentMemorySource::SegmentMemorySource(const std::span<const sys::byte>& data, nitf::Off start,
    int byteSkip, bool copyData)
    : SegmentMemorySource(data.data(), gsl::narrow<nitf::Off>(data.size()), start, byteSkip, copyData)
{
}

static inline const sys::byte* data(const std::span<const std::byte>& data)
{
    const void* pData = data.data();
    return static_cast<const sys::byte*>(pData);
}
template<>
SegmentMemorySource::SegmentMemorySource(const std::span<const std::byte>& s, nitf::Off start,
    int byteSkip, bool copyData)
    : SegmentMemorySource(data(s), gsl::narrow<nitf::Off>(s.size()), start, byteSkip, copyData)
{
}

inline std::span<const std::byte> make_span(const std::vector<std::byte>& data)
{
    return gsl::make_span(data);
}
template<>
SegmentMemorySource::SegmentMemorySource(const std::vector<std::byte>& data,
    nitf::Off start, int byteSkip, bool copyData)
    : SegmentMemorySource(make_span(data), start, byteSkip, copyData)
{
}

inline std::span<const sys::byte> make_span(const std::vector<sys::byte>& data)
{
  return gsl::make_span(data);
}
template<>
SegmentMemorySource::SegmentMemorySource(const std::vector<sys::byte>& data,
    nitf::Off start, int byteSkip, bool copyData)
    : SegmentMemorySource(make_span(data), start, byteSkip, copyData)
{
}
}

nitf::SegmentFileSource::SegmentFileSource(nitf::IOHandle & io,
        nitf::Off start, int byteSkip)
{
    setNative(nitf_SegmentFileSource_constructIO(io.getNative(),
                                                 start, byteSkip,
                                                 &error));
    setManaged(false);
    io.setManaged(true); //TODO unmanage on deletion
}

nitf::SegmentReaderSource::SegmentReaderSource(nitf::SegmentReader reader)
{
    setNative(nitf_SegmentReaderSource_construct(reader.getNativeOrThrow(),
                                                 &error));
    setManaged(false);
    reader.setManaged(true); //TODO unmanage on deletion
}
