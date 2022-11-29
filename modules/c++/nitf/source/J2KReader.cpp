/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include "nitf/J2KReader.hpp"

#include <assert.h>

j2k::details::Reader::Reader(j2k_Reader* x)
{
    setNative(x);
    getNativeOrThrow();
}
j2k::Reader::Reader(j2k_Reader*&& x) : impl_(x) { }

j2k::details::Reader::Reader(nitf::IOHandle& io)
    : Reader(nitf::callNativeOrThrow<j2k_Reader*>(j2k_Reader_openIO, io.getNative())) { }
j2k::Reader::Reader(nitf::IOHandle& io) : impl_(io) { }

j2k::details::Reader::Reader(const std::filesystem::path& fname)
    : Reader(nitf::callNativeOrThrow<j2k_Reader*>(j2k_Reader_open, fname.string().c_str())) { }
j2k::Reader::Reader(const std::filesystem::path& fname) : impl_(fname) { }

j2k::Container j2k::Reader::getContainer() const
{
    return impl_.callNativeOrThrow<j2k_Container*>(j2k_Reader_getContainer);
}

std::span<uint8_t> j2k::Reader::readRegion(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, Buffer& buf)
{
    uint8_t* pBuf = nullptr;
    const auto bufSize = impl_.callNativeOrThrow<uint64_t>(j2k_Reader_readRegion, x0, y0, x1, y1, &pBuf);
    buf = make_Buffer(pBuf); // turn over ownership (and test our utility routine)
    return std::span<uint8_t>(buf.get(), bufSize);
}
std::span<uint8_t> j2k::Reader::readRegion(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, std::span<uint8_t> buf)
{
    // "buf" is already allocated, maybe from a previous call
    auto buf_ = buf.data();
    const auto bufSize = impl_.callNativeOrThrow<uint64_t>(j2k_Reader_readRegion, x0, y0, x1, y1, &buf_);
    return std::span<uint8_t>(buf_, bufSize); // "bufSize" may have changed
}